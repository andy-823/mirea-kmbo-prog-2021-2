#include <iostream>
#include <string>

class Tree;

class Node
{
//public:
    size_t height = 1;
    Node *left, *right, *parent;
    friend class Tree;

public:
    Node *getLeft() { return left; }
    const Node *getLeft() const { return left; }
    Node *getRight() { return right; }
    const Node *getRight() const { return right; }
    Node *getParent() { return parent; }
    const Node *getParent() const { return parent; }

    std::string name;
    std::string description;
    long price;

    Node(const std::string &name_, Node *parent_ = nullptr)
      : left(nullptr), right(nullptr), parent(parent_), name(name_)
    { }

    Node *findMin()
    {
        Node *node = this;
        while (node->left)
            node = node->left;
        return node;
    }

    Node *findMax()
    {
        Node *node = this;
        while (node->right)
            node = node->right;
        return node;
    }
};

class TreeIterator : public std::iterator<std::input_iterator_tag, Node>
{
    Node *node;

public:
    TreeIterator() : node(nullptr) {}
    TreeIterator(Node *node_) : node(node_) {}

    bool operator == (const TreeIterator &other) const { return node == other.node; }
    bool operator != (const TreeIterator &other) const { return node != other.node; }

    Node& operator * () { return *node; }
    const Node& operator * () const { return *node; }

    /// TreeIterator it;  ++it      it++

    TreeIterator& operator++()        /// префиксный   ++it
    {
        if (node->getRight())
        {
            node = node->getRight();
            while (node->getLeft()) node = node->getLeft();
            return *this;
        }

        Node *prev_node = node;
        node = node->getParent();
        while(node)
        {
            if (node->getRight() != prev_node)
            {
                node = node->getRight();
                while (node->getLeft()) node = node->getLeft();
                return *this;
            }
            prev_node = node;
            node = node->getParent();
        }
        return *this;
    }
    TreeIterator operator++(int)      /// постфиксный   it++
    {
        TreeIterator old(node);
        ++*this;
        return old;
    }

    TreeIterator& operator--()
    {
        if (node->getLeft())
        {
            node = node->getLeft();
            while (node->getRight()) node = node->getRight();
            return *this;
        }

        Node *prev_node = node;
        node = node->getParent();
        while(node)
        {
            if (node->getLeft() != prev_node)
            {
                node = node->getLeft();
                while (node->getRight()) node = node->getRight();
                return *this;
            }
            prev_node = node;
            node = node->getParent();
        }
        return *this;
    }
    TreeIterator operator--(int)
    {
        TreeIterator old(node);
        --*this;
        return old;
    }
};

class Tree
{
    Node *root;

    /// Вход: b > a, a - родитель b
    /// Выход: b - родитель a
    void smallTurnLeft(Node *a, Node *b)
    {
        if (a->parent)
            if (a->parent->right == a)
                a->parent->right = b;
            else
                a->parent->left = b;
        b->parent = a->parent;
        a->parent = b;
        if (b->left)
            b->left->parent = a;
        a->right = b->left;
        b->left = a;

        // теперь надо заново сделать высоты
        size_t height_left  = a->left  ? a->left->height  : 0,
               height_right = a->right ? a->right->height : 0;
        a->height = height_left > height_right ? height_left + 1 : height_right + 1;

        height_left  = b->left  ? b->left->height  : 0;
        height_right = b->right ? b->right->height : 0;
        b->height = height_left > height_right ? height_left + 1 : height_right + 1;
        // высота родителя будет откорректирована в балансировке
    }
    /// имеем b < a, a - родитель b
    void smallTurnRight(Node *a, Node*b)
    {
        if (a->parent)
            if (a->parent->right == a)
                a->parent->right = b;
            else
                a->parent->left = b;
        b->parent = a->parent;
        a->parent = b;
        if (b->right)
            b->right->parent = b;
        a->left = b->right;
        b->right = a;

        // теперь надо заново сделать высоты
        size_t height_left  = a->left  ? a->left->height  : 0,
               height_right = a->right ? a->right->height : 0;
        a->height = height_left > height_right ? height_left + 1 : height_right + 1;

        height_left  = b->left  ? b->left->height  : 0;
        height_right = b->right ? b->right->height : 0;
        b->height = height_left > height_right ? height_left + 1 : height_right + 1;
        // высота родителя будет откорректирована в балансировке
    }

    /// имеем a < b, a родитель b
    /// b > c, b - родитель c
    void BigTurnRight(Node *a, Node *b, Node *c)
    {
        smallTurnRight(b, c);
        smallTurnLeft(a, c);
    }
    /// имеем a > b, a - родитель b
    /// b < c, b - родитель c
    void BigTurnLeft(Node *a, Node *b, Node *c)
    {
        smallTurnLeft(b, c);
        smallTurnRight(a, c);
    }

    // дерево считается сбалансированным, если для любого узла
    // высота его правого поддерева отличается от высоты левого не более, чем на 1
    // на самом деле такое условие не гарантирует полной сбалансированности дерева,
    // то есть может оказаться, что есть два таких листовых узла таких,
    // что расстояние от одного узла до корня намного больше расстояния от другого узла до корня
    // возможно здесь есть повторения одного и того же действия
    void BalanceTree(Node *node)
    {
        size_t height_left = 0, height_right = 0;
        int diff;
        Node *currentNode = node;
        do
        {
            height_left  = currentNode->left  ? currentNode->left->height  : 0;
            height_right = currentNode->right ? currentNode->right->height : 0;
            diff = height_left - height_right;

            // высота левого поддерева больше
            if (diff == 2)
            {
                // мы смотрим левый узел данного узла, он, очевидно, не нулевой
                // нужно, чтобы в левом узле высота левого поддерева
                // была не меньше высоты правого
                height_left  = currentNode->left->left  ? currentNode->left->height  : 0;
                height_right = currentNode->left->right ? currentNode->right->height : 0;
                // если 
                if (int(height_left) - int(height_right) < 0)
                    smallTurnLeft(currentNode->left, currentNode->left->right);

                smallTurnRight(currentNode, currentNode->left);
            }
            // высота правого поддерева больше
            if (diff == -2)
            {
                // аналогичные рассуждения, что и для diff = 2
                height_left  = currentNode->right->left  ? currentNode->left->height  : 0;
                height_right = currentNode->right->right ? currentNode->right->height : 0;
                if (int(height_left) - int(height_right) > 0)
                    smallTurnRight(currentNode->right, currentNode->right->left);

                smallTurnLeft(currentNode, currentNode->right);
            }

            height_left  = currentNode->left  ? currentNode->left->height  : 0;
            height_right = currentNode->right ? currentNode->right->height : 0;
            currentNode->height = height_left > height_right ? height_left : height_right;
            currentNode->height++;
            
            currentNode = currentNode->parent;
        } while (currentNode);
    }
public:
    Node *getRoot() { return root; }
    const Node *getRoot() const { return root; }

    Tree() : root(nullptr) { }
    Node *addNode(const std::string &name)
    {
        Node *closest = findClosest(name);
        if (closest && closest->name == name)
            return nullptr;
        Node *newNode = new Node(name, closest);

        if (!root)
        {
            root = newNode;
            return newNode;
        }

        if (!closest) /// значит наш элемент больше максимального
        {
            /// добавляем самый правый элемент
            Node *prevMax = root->findMax();
            prevMax->right = newNode;
            newNode->parent = prevMax;
        }
        else /// нашелся тот, что правее
        {
            /// добавляем левый от ближайшего
            closest->left = newNode;
            newNode->parent = closest;
        }
        // нужно обновить высоту в родителях
        Node *currentNode = newNode;
        while (currentNode->parent && currentNode->parent->height == currentNode->height)
        {
            currentNode->parent->height++;
            currentNode = currentNode->parent;
        }
        BalanceTree(newNode);
        return newNode;
    }

    Node *findNode(const std::string &name)
    {
        for (auto node = root; node;)
        {
            auto res = name.compare(node->name);
            if (res == 0)
                return node;
            if (res < 0)
                node = node->left;
            else
                node = node->right;
        }
        return nullptr;
    }

    Node *findMin()
    {
        Node *node = root;
        if (!node)
            return nullptr;
        while (node->left)
            node = node->left;
        return node;
    }

    Node *findMax()
    {
        Node *node = root;
        if (!node)
            return nullptr;
        while (node->right)
            node = node->right;
        return node;
    }

    /// Ищет узел с таким же, или максимально близким справа ("большим") значением name.
    Node *findClosest(const std::string &name)
    {
        Node *closest = nullptr;
        for (auto node = root; node;)
        {
            auto res = name.compare(node->name);
            if (res == 0)
                return node;
            if (res > 0)
                node = node->right;
            else
            {
                closest = node;
                node = node->left;
            }
        }
        return closest;        
    }

    void deleteNode(Node *node)
    {
        // похоже, что предполагается, что этот узел есть в дереве
        // найдем ближайший: такой, что если поменяем на него, то ничего не изменится
        // обязательное условие: он должен быть листовой
        Node *closest = nullptr, *balanceFrom = node;
        // ищем самый левый в правом поддереве
        for (auto tmp = node->right; tmp;)
        {
            closest = tmp;
            tmp = tmp->left;
        }
        // справа ничего не было, слева не более одного узла
        // нужно еще предка подправить
        if (!closest)
        {
            // слева узел есть
            if (node->left)
            {
                node->name = node->left->name;
                delete node->left;
                node->left = nullptr;
            }
            else
            {
                if (!node->parent)
                    delete node;
                else if (node->parent->right == node)
                {
                    balanceFrom = node->parent;

                    node->parent->right = nullptr;
                    delete node;
                }
                else
                {
                    balanceFrom = node->parent;

                    node->parent->left = nullptr;
                    delete node;
                }
            }
        }
        else
        {
            node->name = closest->name;
            //у closest родитель точно есть
            balanceFrom = closest->parent;
            if (closest->parent->right == closest)
                closest->parent->right = nullptr;
            else
                closest->parent->left = nullptr;
            delete closest;
        }

        BalanceTree(balanceFrom);
    }

    TreeIterator begin() {}     /// Возвращает итератор, указывающий на минимальный элемент
    TreeIterator end() {}       /// Возвращает итератор, указывающий на nullptr Node
};


#include <cassert>

void testAddNode() {
    Tree *result = new Tree();

    auto abc = result->addNode("Abc");
    assert(abc != nullptr);
    assert(abc->name == "Abc");
    assert(abc->description == "");
    assert(abc->getParent() == nullptr);
    assert(abc->getLeft() == nullptr);
    assert(abc->getRight() == nullptr);

    auto def = result->addNode("Def");
    assert(def != nullptr);
    assert(def->name == "Def");
    assert(def->description == "");
    assert(def->getParent() == abc);
    assert(abc->getRight() == def);

    /// Проверяем малый левый поворот
    auto ghi = result->addNode("Ghi");
    assert(ghi != nullptr);
    assert(ghi->name == "Ghi");
    assert(ghi->description == "");
    assert(ghi->getParent() == def);
    assert(ghi->getLeft() == nullptr);
    assert(ghi->getRight() == nullptr);
    assert(def->getParent() == nullptr);
    assert(def->getLeft() == abc);
    assert(def->getRight() == ghi);
    assert(abc->getParent() == def);
    assert(abc->getLeft() == nullptr);
    assert(abc->getRight() == nullptr);

    /// Задача: дописать тест, проверяя малый правый и оба больших поворота
    /// https://gist.github.com/grayed
}

#include <map>
#include <set>

int main()
{
    Tree tree;
    tree.addNode("First");
    tree.addNode("Second");
    tree.addNode("Third");
    std::cout << "Our tree:" << std::endl;
    for (auto &node : tree) {
        std::cout << node.name << ": " << node.description << std::endl;
    }

    const Node *n = tree.getRoot();

    std::set<std::string> setOfStrings;
    std::map<std::string, std::string> myMap;

    setOfStrings.insert("First");
    setOfStrings.insert("Second");
    setOfStrings.insert("Third");
    setOfStrings.insert("First");
    setOfStrings.insert("First");
    std::cout << "Set contains:" << std::endl;
    for (auto s : setOfStrings)
        std::cout << s << std::endl;
    std::cout << std::endl;

    // этот тест был успешно пройден
    testAddNode();


    system("pause");
    return 0;
}

