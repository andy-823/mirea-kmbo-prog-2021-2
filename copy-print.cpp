#include <cstdint>
#include <iostream>
#include <cstdlib>

// Домашнее задание:
//
// 1. Дореализовать copy_array() и print_array()
//
// 2. Реализовать функцию поиска минимума в заданном массиве:
//       const T *where, size_t count, const T &what
//     Должна возвращать индекс найденного элемента (-1 если не найден,
//     но 0 для пустого массива).

using namespace std;

template<typename T>
void copy_array(const T *from, T *to, size_t count)
{
    if (count == 0) return;
    for (int64_t i = 0; i < count; i++)
        to[i] = from[i];
}

template<typename T>
void print_array(const T *parr, size_t count)
{
    if (count == 0) return;
    for (int i = 0; i < count; i++)
        cout << parr[i] << " ";
    cout << endl;
}

// здесь некорректно условие, не ясно, что искать: индекс минимума или индекс элемента с указанным значением
// я ищу минимум
template<typename T>
size_t find_min(const T *where, size_t count)
{
    if (count == 0) return 0;
    T res = where[count - 1];
    while (--count)
        res = min(res, where[count - 1]);
    return res;
}

int main(int argc, char **argv) {
    int arr[20], arr2[20];
    for (auto i = 0; i < sizeof(arr)/sizeof(arr[0]); i++)
        arr[i] = rand();
    copy_array(arr, arr2, sizeof(arr)/sizeof(arr[0]));
    print_array(arr, sizeof(arr)/sizeof(arr[0]));
    print_array(arr2, sizeof(arr2)/sizeof(arr2[0]));
    cout << "\n";
    cout << find_min(arr, sizeof(arr)/sizeof(arr[0]));
    //system("pause");
    return 0;
}
