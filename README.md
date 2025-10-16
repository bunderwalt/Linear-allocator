# Linear Memory Allocator

Перед прочтением советую хорошенько познакомиться с выравниванием памяти, виртуальной памятью. Для понимания прицнипа работы linear allocator можно прочитать это статью https://habr.com/ru/articles/505632/


## Особенности

- ✅ Линейное выделение памяти O(1)
- ✅ Поддержка выравнивания памяти
- ✅ Работа через системные вызовы (mmap/munmap)
- ✅ Статистика использования памяти
- ✅ Полное тестирование

## Структура проекта

linear_allocator/

├── linear_allocator.h # Интерфейс аллокатора

├── linear_allocator.c # Реализация

├── main.c # Тесты и примеры

├── CMakeLists.txt # Конфигурация сборки

└── README.md # Документация


## Использование

```c
#include "linear_allocator.h"

// Создание аллокатора на 1MB
LinearAllocator* alloc = create_allocator(1024 * 1024);

// Выделение памяти
int* numbers = linear_allocate(alloc, 100 * sizeof(int), 4);

// Использование...
for (int i = 0; i < 100; i++) {
    numbers[i] = i;
}

// Освобождение
destroy_allocator(alloc);
