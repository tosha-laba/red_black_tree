#pragma once
#include <iostream>
#include <sstream>

namespace rb_tree
{
    template <typename T>
    class tree
    {
    private:
        enum class node_color { red,
                                black };

        struct node {
            node *parent = nullptr;
            node *left_child = nullptr;
            node *right_child = nullptr;
            node_color color = node_color::red;
            T key;

            ~node()
            {
                if (left_child != nullptr)
                    delete left_child;

                if (right_child != nullptr)
                    delete right_child;
            }
        };

        node *root = nullptr;

        // Ищет "дедушку" узла n
        node *grandparent(node *n)
        {
            if (n == nullptr)
                return nullptr;

            if (n->parent == nullptr)
                return nullptr;

            return n->parent->parent;
        }

        // Ищет "дядю" узла n
        node *uncle(node *n)
        {
            node *g = grandparent(n);

            if (g == nullptr) {
                return nullptr;
            }

            if (n->parent == g->left_child) {
                return g->right_child;
            }

            return g->left_child;
        }

        // Ищет "брата" узла n с родителем parent
        node *sibling(node *n, node *parent)
        {
            if (parent == nullptr) {
                return nullptr;
            }

            if (n == parent->left_child) {
                return parent->right_child;
            }

            return parent->left_child;
        }

        // Левый поворот относительно узла x
        void left_rotate(node *x)
        {
            if (x == nullptr)
                return;
            if (x->right_child == nullptr)
                return;

            node *y = x->right_child;
            x->right_child = y->left_child;

            if (y->left_child != nullptr) {
                y->left_child->parent = x;
            }

            y->parent = x->parent;
            if (x->parent == nullptr) {
                root = y;
            } else {
                if (x == x->parent->left_child) {
                    x->parent->left_child = y;
                } else {
                    x->parent->right_child = y;
                }
            }

            y->left_child = x;
            x->parent = y;
        }

        // Правый поворот относительно узла x
        void right_rotate(node *x)
        {
            if (x == nullptr)
                return;
            if (x->left_child == nullptr)
                return;

            node *y = x->left_child;
            x->left_child = y->right_child;

            if (y->right_child != nullptr) {
                y->right_child->parent = x;
            }

            y->parent = x->parent;
            if (x->parent == nullptr) {
                root = y;
            } else {
                if (x == x->parent->left_child) {
                    x->parent->left_child = y;
                } else {
                    x->parent->right_child = y;
                }
            }

            y->right_child = x;
            x->parent = y;
        }

        // Проверяет, является ли z красным
        bool is_red(node *z)
        {
            if (z == nullptr) {
                return false;
            }

            return z->color == node_color::red;
        }

        void insert_case_1(node *);
        void insert_case_2(node *);
        void insert_case_3(node *);
        void insert_case_4(node *);
        void insert_case_5(node *);

        // Восстановление красно-чёрных свойств при добавлении узла
        void insert_fixup(node *z)
        {
            insert_case_1(z);
        }

        void remove_case_1(node *, node *);
        void remove_case_2(node *, node *);
        void remove_case_3(node *, node *);
        void remove_case_4(node *, node *);
        void remove_case_5(node *, node *);
        void remove_case_6(node *, node *);

        // Восстановление красно-чёрных свойств при удалении узла
        void remove_fixup(node *x, node *parent = nullptr)
        {
            remove_case_1(x, parent);
        }

    public:
        ~tree()
        {
            if (root != nullptr)
                delete root;
        }

        // Добавление в дерево элемента с ключом item
        void insert(T item)
        {
            node *current = root;
            node *previous = nullptr;

            while (current != nullptr) {
                previous = current;

                if (item < current->key) {
                    current = current->left_child;
                } else {
                    current = current->right_child;
                }
            }

            node *inserting = new node;

            inserting->key = item;
            inserting->color = node_color::red;
            inserting->parent = previous;

            if (previous == nullptr) {
                root = inserting;
            } else {
                if (item < previous->key) {
                    previous->left_child = inserting;
                } else {
                    previous->right_child = inserting;
                }
            }

            insert_fixup(inserting);
        }

        // Поиск минимума от узла current
        node *minimum(node *current = root)
        {
            if (current == nullptr)
                return current;

            while (current->left_child != nullptr) {
                current = current->left_child;
            }

            return current;
        }

        // Поиск максимума от узла current
        node *maximum(node *current = root)
        {
            if (current == nullptr)
                return current;

            while (current->right_child != nullptr) {
                current = current->right_child;
            }

            return current;
        }

        // Поиск в дереве элемента с ключом key
        node *find(T key)
        {
            node *current = root;

            for (;;) {
                if (current == nullptr)
                    return current;

                if (current->key == key)
                    return current;

                if (key < current->key) {
                    current = current->left_child;
                } else {
                    current = current->right_child;
                }
            }
        }

        // Поиск в дереве элемента с ключом key,
        // который находится глубже, чем первое вхождение
        // элемента с таким ключом в дерево.
        node *find_weak(T key)
        {
            node *search_result = find(key);

            node *pred = predecessor_of(search_result);
            if (pred != nullptr && pred->key == key)
                return pred;

            node *succ = successor_of(search_result);
            if (succ != nullptr && succ->key == key)
                return succ;

            return search_result;
        }

        // Поиск идущего перед current узла дерева
        node *predecessor_of(node *current)
        {
            if (current == nullptr)
                return current;

            if (current->left_child != nullptr)
                return maximum(current->left_child);

            node *upward = current->parent;

            while (upward != nullptr && current == upward->left_child) {
                current = upward;
                upward = current->parent;
            }

            return upward;
        }

        // Поиск идущего после current узла дерева
        node *successor_of(node *current)
        {
            if (current == nullptr)
                return current;

            if (current->right_child != nullptr)
                return minimum(current->right_child);

            node *upward = current->parent;

            while (upward != nullptr && current == upward->right_child) {
                current = upward;
                upward = current->parent;
            }

            return upward;
        }

        // Удаление узла из дерева
        node *remove(node *current)
        {
            if (current == nullptr)
                return current;

            // Выбор удаляемого элемента
            node *removable = nullptr;
            if (current->left_child == nullptr || current->right_child == nullptr) {
                removable = current;
            } else {
                removable = successor_of(current);
            }

            // Поиск потомка удаляемого элемента
            node *child = nullptr;
            if (removable != nullptr) {
                if (removable->left_child != nullptr) {
                    child = removable->left_child;
                } else {
                    child = removable->right_child;
                }
            }

            // Исключаем элемент путем изменения указателей
            // Кормен говорит, что у КЧД с ограничителем присваивание выполняется безусловно
            // Пока не понятно, где это используется.
            // Хорошо было бы, если бы это было просто из-за природы ограничителей.
            if (child != nullptr) {
                child->parent = (removable != nullptr ? removable->parent : nullptr);
            }

            if (removable == nullptr || removable->parent == nullptr) {
                root = child;
            } else {
                if (removable == removable->parent->left_child) {
                    removable->parent->left_child = child;
                } else {
                    removable->parent->right_child = child;
                }
            }

            // Если удаляем не конкретный элемент, а следующий за ним,
            // то копируем данные (не служебные) в оставшийся элемент
            if (removable != current) {
                if (removable != nullptr) {
                    current->key = removable->key;
                }
            }

            // Если узел чёрный, то могли быть нарушены КЧ-свойства
            // => необходимо их восстановить
            if (removable != nullptr && removable->color == node_color::black) {
                if (is_red(child)) {
                    child->color = node_color::black;
                } else {
                    node *parent = (child != nullptr ? child->parent : nullptr);
                    remove_fixup(child, parent);
                }
            }

            if (removable != nullptr) {
                removable->left_child = nullptr;
                removable->right_child = nullptr;
            }

            return removable;
        }

        void traverse(node *current, int indent, std::ofstream &out)
        {
            if (current == nullptr)
                return;

            out << '\n';

            for (int i = 0; i < indent; ++i) {
                out << '\t';
            }

            out << "key:" << current->key << "; color: " << (current->color == node_color::black ? "black" : "red");

            traverse(current->left_child, indent + 1, out);
            traverse(current->right_child, indent + 1, out);
        }

        // Симметричный обход дерева с выводом информации о нём
        void traverse(std::ofstream &out = std::cout)
        {
            traverse(root, 0, out);
        }

        std::string traverse_html(node *current)
        {
            if (current == nullptr)
                return "";

            bool add_ul = current->left_child != nullptr || current->right_child != nullptr;

            std::string color = is_red(current) ? "red" : "black";

            std::stringstream ret_ss;

            ret_ss << "<li><a id='" + color + "'>";
            ret_ss << current->key;
            ret_ss << "</a>";

            if (add_ul) {
                ret_ss << "<ul>";
            }

            ret_ss << traverse_html(current->left_child);
            ret_ss << traverse_html(current->right_child);

            if (add_ul) {
                ret_ss << "</ul>";
            }

            ret_ss << "</li>";

            return ret_ss.str();
        }

        // Сериализация дерева в многоуровневый список HTML
        std::string traverse_html()
        {
            return "<div class='tree'><ul>" + traverse_html(root) + "</ul></div>";
        }
    };

    template <typename T>
    void tree<T>::insert_case_1(node *z)
    {
        if (z == nullptr) {
            return;
        }

        if (z->parent == nullptr) {
            z->color = node_color::black;
        } else {
            insert_case_2(z);
        }
    }

    template <typename T>
    void tree<T>::insert_case_2(node *z)
    {
        if (z->parent->color == node_color::black) {
            return;
        } else {
            insert_case_3(z);
        }
    }

    template <typename T>
    void tree<T>::insert_case_3(node *z)
    {
        node *u = uncle(z);

        if (is_red(u)) {
            z->parent->color = node_color::black;
            u->color = node_color::black;

            node *g = grandparent(z);
            g->color = node_color::red;
            insert_case_1(g);
        } else {
            insert_case_4(z);
        }
    }

    template <typename T>
    void tree<T>::insert_case_4(node *z)
    {
        node *g = grandparent(z);

        if ((z == z->parent->right_child) && (z->parent == g->left_child)) {
            left_rotate(z->parent);
            z = z->left_child;
        } else if ((z == z->parent->left_child) && (z->parent == g->right_child)) {
            right_rotate(z->parent);
            z = z->right_child;
        }

        insert_case_5(z);
    }

    template <typename T>
    void tree<T>::insert_case_5(node *z)
    {
        node *g = grandparent(z);

        z->parent->color = node_color::black;
        g->color = node_color::red;

        if ((z == z->parent->left_child) && (z->parent == g->left_child)) {
            right_rotate(g);
        } else {
            left_rotate(g);
        }
    }

    template <typename T>
    void tree<T>::remove_case_1(node *z, node *parent)
    {
        if (parent != nullptr) {
            remove_case_2(z, parent);
        }
    }

    template <typename T>
    void tree<T>::remove_case_2(node *z, node *parent)
    {
        node *s = sibling(z, parent);

        if (is_red(s)) {
            parent->color = node_color::red;
            s->color = node_color::black;

            if (z == parent->left_child) {
                left_rotate(parent);
            } else {
                right_rotate(parent);
            }
        }
        remove_case_3(z, parent);
    }

    template <typename T>
    void tree<T>::remove_case_3(node *z, node *parent)
    {
        node *s = sibling(z, parent);

        if (!is_red(parent) &&
            s != nullptr &&
            s->color == node_color::black &&
            !is_red(s->left_child) &&
            !is_red(s->right_child)) {
            s->color = node_color::red;
            remove_case_1(parent, parent->parent);
        } else {
            remove_case_4(z, parent);
        }
    }

    template <typename T>
    void tree<T>::remove_case_4(node *z, node *parent)
    {
        node *s = sibling(z, parent);

        if (is_red(parent) &&
            s != nullptr &&
            s->color == node_color::black &&
            !is_red(s->left_child) &&
            !is_red(s->right_child)) {
            s->color = node_color::red;
            parent->color = node_color::black;
        } else {
            remove_case_5(z, parent);
        }
    }

    template <typename T>
    void tree<T>::remove_case_5(node *z, node *parent)
    {
        node *s = sibling(z, parent);

        if (s != nullptr && (s->color == node_color::black)) {
            if (z == parent->left_child &&
                !is_red(s->right_child) &&
                is_red(s->left_child)) {
                s->color = node_color::red;
                s->left_child->color = node_color::black;
                right_rotate(s);
            } else if (z == parent->right_child &&
                       !is_red(s->left_child) &&
                       is_red(s->right_child)) {
                s->color = node_color::red;
                s->right_child->color = node_color::black;
                left_rotate(s);
            }
        }
        remove_case_6(z, parent);
    }

    template <typename T>
    void tree<T>::remove_case_6(node *z, node *parent)
    {
        node *s = sibling(z, parent);

        if (s != nullptr) {
            s->color = parent->color;
            parent->color = node_color::black;

            if (z == parent->left_child) {
                s->right_child->color = node_color::black;
                left_rotate(parent);
            } else {
                s->left_child->color = node_color::black;
                right_rotate(parent);
            }
        }
    }

} // namespace rb_tree
