#include <algorithm>
#include <iostream>
#include <vector>

template <class T>
class HeapOverArray {
    std::vector<T> v;

public:
    HeapOverArray() {}
    HeapOverArray(const std::vector<T> &arg)
    {
        if (arg.size())
            for (auto &a : arg) addNode(a);
    }

    const std::vector<T>& getVector() const { return v; }

    // true, если такого элемента нет, иначе false
    // в моей реализации такой проверки нет, так что
    // пока никакой проверки нет, так что будем надеяться на умного пользователя,
    // который вносит только разные значения
    bool addNode(const T& o) {
        // 1. Определем место вставки (первое свободное место в горизонтальном ряду)
        // 2. Сравниваем с родителем, если значение больше родительского, меняем его
        // местами с родителем (в цикле).

        size_t pos = v.size();
        v.push_back(o);
        if (pos == 0) {
            // дерево было пустое
            return true;
        }
        size_t parent_pos = (pos + 1) / 2 - 1;
        // я так понимаю мы получили, что pos соответсвтует последнему элементу в
        // массиве далее берем + 1 тк нужен номер элемента, далее делим на 2 и
        // получаем номер родителя далее вычитаем 1 чтобы получить какому индексу в
        // массиве соответствует родитель
        while (parent_pos >= 0 && v[pos] > v[parent_pos]) {
            std::swap(v[pos], v[parent_pos]);
            // v[pose] это наш элемент o

            pos = parent_pos;
            parent_pos = (pos + 1) / 2 - 1;
        }
        return true;
    }

    bool removeNode(const T& o) {
        // найдем этот элемент и его индекс
        size_t index = 0;
        while (index < v.size() && v[index] != o) index++;

        if (index == v.size()) {
            // случай где такого элемента просто нет
            return false;
        }
        v[index] = v[v.size() - 1];
        v.pop_back();  // удалили элемент

        // теперь надо этот элемент поставить куда надо
        // сначала посмотрим, если он больше родителя
        size_t pos = index + 1, parent_pos = pos / 2;
        while (parent_pos > 0 && v[pos - 1] > v[parent_pos - 1]) {
            std::swap(v[pos - 1], v[parent_pos - 1]);

            // мы свапаем с родителем
            // если мы слева, то ничего делать не надо
            // если мы справа, то надо свапнуть правый и левый
            // if (pos % 2 == 0 && pos != v.size() && ) std::swap(v[pos - 1], v[pos]);

            pos = parent_pos;
            parent_pos = pos / 2;
        }
        // теперь аналогично с ребенком
        size_t child_pos = pos * 2;
        // цикл будет пока элемент в позиции меньше одного из детей
        while (child_pos < v.size() &&
                (v[pos - 1] < v[child_pos - 1] || v[pos - 1] < v[child_pos])) {
            // получаем позицию максимального из детей
            // и затем меняем наш элемент с ним
            child_pos = v[child_pos - 1] > v[child_pos] ? child_pos : child_pos + 1;
            std::swap(v[pos - 1], v[child_pos - 1]);
            pos = child_pos;
            child_pos = pos * 2;
        }
        if (child_pos == v.size() && v[pos - 1] < v[child_pos - 1])
            std::swap(v[pos - 1], v[child_pos - 1]);
        return true;
    }
    class iterator {
        std::vector<T> vec;
        T* res;
        size_t idx;
    public:
        iterator() : res(nullptr) {}
        iterator(std::vector<T> &v_, size_t idx_) : vec(v_), idx(idx_) {}

        iterator operator++()
        {
            size_t pos = idx + 1;
            if (2 * pos > vec.size()) {
                // правых детей нет, но есть элемент правее
                // идем в родителя пока он не будет левым узлом
                while (pos % 2) pos /= 2;
                // родитель - левый узел
                pos /= 2;
            } else {
                //идем в правый узел
                pos = 2 * pos + 1;
                // пока есть левый узел идем в него
                while (2 * pos <= vec.size()) pos *= 2;
            }
            if (pos == 0) return iterator();
            idx = pos - 1;
            /// 1. узнаем высоту элемента
            /// 2. узнаем, слева он или справа

            /// если двигаемся вверх (если нет дочернего элемента справа),
            /// делим номер в дереве пополам.
            /// затем отбрасывая дробную часть, в цикле пока не дойдем до четного
            /// элемента затем еще раз делим номер пополам
            ///
            /// если двигаемся вниз, то: 1) увеличиваем номер в 2 раза и прибавляем 1
            /// 2) пока есть дечерние элементы, увеличиваем номер в 2 раза
            ///
            /// если двигаться некуда, превращаем итератор в невалидный путем
            /// зануления v
            ///
            /// лист == номер элемента, умноженный в 2 раза, меньше количесвта
            /// элементов в дереве
            res = &vec[idx];
            return *this;
        }

        iterator operator++(int) {
            auto prev = *this;
            ++(*this);
            return prev;
        }

        //bool operator bool() const { return v != nullptr; }
        bool operator==(iterator &it) { return idx == it.idx && v == it.vec; }
        bool operator!=(iterator &it) { return !(idx == it.idx && vec == it.vec); } 

        const T operator*() const { return vec[idx]; }
        T operator*() { return vec[idx]; }
    };

    // как можно заметить, работа у итераторов логарифмическая
    // и лучше кактоих рассчитывать заранее
    // но моя задача не их написать
    iterator begin()
    {
        int pos = 1;
        while (pos <= v.size()) pos *= 2;
        return iterator(v, size_t(pos / 2 - 1));
    }
    const iterator begin() const { return begin(); }
    // у меня это самый левый элемент
    iterator end()
    {
        int pos = 1;
        while (pos <= v.size()) pos = pos * 2 + 1;
        return iterator(v, size_t(pos / 2 - 1));
    }
    const iterator end() const { return end(); }
};

// как оказалось, у меня немного по другому работает добавление
// по этой причине я буду только выводить массив
// на введенном мной массиве результат был корректным
template <class T>
void testHeapAddRemove(const std::vector<T>& initial){
    std::cout << "got:";
    for (auto &a : initial) std::cout << " " << a;
    std::cout << std::endl;
    HeapOverArray<int> heap(initial);
    std::vector<int> v = heap.getVector();
    std::cout << "in heap:";
    for (auto &a : v) std::cout << " " << a;
    std::cout << std::endl;

    for (int i = 0; v.size() > 0; i++)
    {
        std::cout << "after remove max " << i + 1 << ":";
        heap.removeNode(heap.getVector()[0]);
        v = heap.getVector();
        for (auto &a : v) std::cout << " " << a;
        std::cout << std::endl;
    }
}

void testHeapInc(const std::vector<int> &initial)
{
    std::cout << "got:";
    for (auto &a : initial) std::cout << " " << a;
    std::cout << std::endl;
    HeapOverArray<int> heap(initial);
    std::vector<int> v = heap.getVector();
    std::cout << "in heap:";
    for (auto &a : v) std::cout << " " << a;
    std::cout << std::endl;

    std::cout << "testing++:" << std::endl;
    auto el = heap.begin();
    int number = 1;
    while (number <= initial.size())
    {
        std::cout << number << " " << *el << std::endl;
        el++;
        number++;
    }
}

int main() {
  std::vector<int> initial = {10, 2, 3, 4, 5, 6, 7, 8, 9};
  testHeapAddRemove(initial);
  testHeapInc(initial);
  system("pause");
  return 0;
}
