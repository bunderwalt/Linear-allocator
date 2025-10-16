#include "linear_allocator.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

// Получение размера страницы системы
static size_t get_page_size() {
    static size_t page_size = 0;
    if (page_size == 0) {
        page_size = sysconf(_SC_PAGESIZE);
    }
    return page_size;
}

LinearAllocator* create_allocator(size_t size) {
    if (size == 0) return NULL;
    
    size_t page_size = get_page_size();
    size_t total_needed = sizeof(LinearAllocator) + size;
    size_t total_size = (total_needed + page_size - 1) & ~(page_size - 1);

    void* memory = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memory == MAP_FAILED) return NULL;

    LinearAllocator* allocator = (LinearAllocator*)memory;
    allocator->start = (char*)memory + sizeof(LinearAllocator);
    allocator->current = allocator->start;
    allocator->total_size = total_size - sizeof(LinearAllocator);
    allocator->used = 0;

    printf("✅ Создан аллокатор: %zu байт (пользователю: %zu байт)\n",
           total_size, allocator->total_size);

    return allocator;
}

void* linear_allocate(LinearAllocator* allocator, size_t block_size, size_t alignment) {
    if (block_size == 0 || allocator == NULL) return NULL;
    
    // Проверка что alignment - степень двойки
    if ((alignment & (alignment - 1)) != 0) {
        return NULL;
    }

    uintptr_t current_addr = (uintptr_t)allocator->current;
    uintptr_t aligned_addr = (current_addr + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned_addr - current_addr;

    size_t aligned_size = (block_size + alignment - 1) & ~(alignment - 1);

    void* future_position = (char*)aligned_addr + aligned_size;
    void* pool_end = (char*)allocator->start + allocator->total_size;

    if (future_position > pool_end) {
        printf("❌ Не хватило места: нужно %zu, доступно %zu\n", 
               aligned_size, (size_t)((char*)pool_end - (char*)allocator->current));
        return NULL;
    }

    void* result = (void*)aligned_addr;
    allocator->current = future_position;
    allocator->used += aligned_size + padding;

    printf("✅ Выделено: %zu байт (выровненных: %zu), использовано: %zu/%zu\n",
           block_size, aligned_size, allocator->used, allocator->total_size);

    return result;
}

void linear_free(LinearAllocator* allocator) {
    if (allocator) {
        allocator->current = allocator->start;
        allocator->used = 0;
        printf("✅ Память сброшена\n");
    }
}

void destroy_allocator(LinearAllocator* allocator) {
    if (allocator) {
        size_t total_allocated = allocator->total_size + sizeof(LinearAllocator);
        munmap(allocator, total_allocated);
        printf("✅ Аллокатор уничтожен (%zu байт возвращено ОС)\n", total_allocated);
    }
}

// Вспомогательные функции
size_t get_allocator_available_space(const LinearAllocator* allocator) {
    if (!allocator) return 0;
    void* pool_end = (char*)allocator->start + allocator->total_size;
    return (size_t)((char*)pool_end - (char*)allocator->current);
}

size_t get_allocator_used_space(const LinearAllocator* allocator) {
    return allocator ? allocator->used : 0;
}

size_t get_allocator_total_space(const LinearAllocator* allocator) {
    return allocator ? allocator->total_size : 0;
}