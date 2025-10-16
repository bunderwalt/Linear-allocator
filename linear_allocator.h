#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    // Структура линейного аллокатора
    typedef struct {
        void* start;
        void* current;
        size_t total_size;
        size_t used;
    } LinearAllocator;

    // Базовые функции управления аллокатором
    LinearAllocator* create_allocator(size_t size);
    void destroy_allocator(LinearAllocator* allocator);

    // Функции работы с памятью
    void* linear_allocate(LinearAllocator* allocator, size_t block_size, size_t alignment);
    void linear_free(LinearAllocator* allocator);

    // Вспомогательные функции
    size_t get_allocator_available_space(const LinearAllocator* allocator);
    size_t get_allocator_used_space(const LinearAllocator* allocator);
    size_t get_allocator_total_space(const LinearAllocator* allocator);

#ifdef __cplusplus
}
#endif

#endif // LINEAR_ALLOCATOR_H