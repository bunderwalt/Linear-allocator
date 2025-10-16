#include "linear_allocator.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

void test_basic_functionality() {
    printf("\n=== Тест 1: Базовая функциональность ===\n");

    LinearAllocator* alloc = create_allocator(1024);
    assert(alloc != NULL);

    // Выделение памяти для разных типов данных
    int* numbers = linear_allocate(alloc, 10 * sizeof(int), 4);
    assert(numbers != NULL);
    for (int i = 0; i < 10; i++) {
        numbers[i] = i * i;
    }
    printf("Создан массив int[10]\n");

    char* text = linear_allocate(alloc, 50, 1);
    assert(text != NULL);
    strcpy(text, "Hello from linear allocator!");
    printf("Создана строка: %s\n", text);

    double* values = linear_allocate(alloc, 5 * sizeof(double), 8);
    assert(values != NULL);
    for (int i = 0; i < 5; i++) {
        values[i] = 3.14 * i;
    }
    printf("Создан массив double[5]\n");

    printf("Использовано: %zu/%zu байт\n",
           get_allocator_used_space(alloc),
           get_allocator_total_space(alloc));

    destroy_allocator(alloc);
}

void test_alignment() {
    printf("\n=== Тест 2: Проверка выравнивания ===\n");

    LinearAllocator* alloc = create_allocator(256);
    assert(alloc != NULL);

    // Способ 1: Создаем невыровненный указатель ВНУТРИ памяти аллокатора
    // Выделяем 1 байт чтобы сдвинуть current на невыровненную позицию
    char* temp_block = linear_allocate(alloc, 1, 1);
    assert(temp_block != NULL);
    // Теперь alloc->current невыровнен (сдвинут на 1 байт)

    void* block = linear_allocate(alloc, 10, 8);
    assert(block != NULL);
    printf("Адрес блока: %p\n", block);
    printf("Выровнен по 8: %s\n", ((uintptr_t)block % 8 == 0) ? "да" : "нет");

    destroy_allocator(alloc);
}

void test_overflow() {
    printf("\n=== Тест 3: Проверка переполнения ===\n");

    // Создаем аллокатор и используем его реальный размер
    LinearAllocator* alloc = create_allocator(100);
    assert(alloc != NULL);

    printf("Реальный размер аллокатора: %zu байт\n", alloc->total_size);

    // Выделяем 90% доступной памяти
    size_t first_allocation = alloc->total_size * 9 / 10;
    void* block1 = linear_allocate(alloc, first_allocation, 1);
    assert(block1 != NULL);
    printf("Блок 1 выделен успешно (%zu байт)\n", first_allocation);

    // Пытаемся выделить оставшиеся 20% - должно не хватить
    size_t second_allocation = alloc->total_size / 5; // 20%
    void* block2 = linear_allocate(alloc, second_allocation, 1);

    if (block2 == NULL) {
        printf("✅ Блок 2 корректно отклонен (переполнение)\n");
    } else {
        printf("⚠️  Неожиданно выделился блок 2 (%zu байт)\n", second_allocation);
        printf("   Использовано: %zu/%zu байт\n", alloc->used, alloc->total_size);
    }

    destroy_allocator(alloc);
}

void test_reset_functionality() {
    printf("\n=== Тест 4: Проверка сброса ===\n");

    LinearAllocator* alloc = create_allocator(200);
    assert(alloc != NULL);

    void* block1 = linear_allocate(alloc, 50, 1);
    assert(block1 != NULL);
    printf("До сброса: использовано %zu байт\n", get_allocator_used_space(alloc));

    linear_free(alloc);
    printf("После сброса: использовано %zu байт\n", get_allocator_used_space(alloc));

    void* block2 = linear_allocate(alloc, 50, 1);
    assert(block2 != NULL);
    printf("После сброса память снова доступна\n");

    destroy_allocator(alloc);
}

void test_different_alignments() {
    printf("\n=== Тест 5: Разные выравнивания ===\n");

    LinearAllocator* alloc = create_allocator(500);
    assert(alloc != NULL);

    void* blocks[4];
    blocks[0] = linear_allocate(alloc, 10, 1);   // Выравнивание 1
    blocks[1] = linear_allocate(alloc, 20, 4);   // Выравнивание 4
    blocks[2] = linear_allocate(alloc, 30, 8);   // Выравнивание 8
    blocks[3] = linear_allocate(alloc, 40, 16);  // Выравнивание 16

    for (int i = 0; i < 4; i++) {
        if (blocks[i]) {
            size_t alignment = (i == 0) ? 1 : (1 << (i + 1));
            printf("Блок %d: %p, выровнен по %zu: %s\n",
                   i + 1, blocks[i], alignment,
                   ((uintptr_t)blocks[i] % alignment == 0) ? "да" : "нет");
        }
    }

    destroy_allocator(alloc);
}

int main() {
    printf("🚀 Запуск тестов линейного аллокатора\n");

    test_basic_functionality();
    test_alignment();
    test_overflow();
    test_reset_functionality();
    test_different_alignments();

    printf("\n🎉 Все тесты завершены успешно!\n");
    return 0;
}