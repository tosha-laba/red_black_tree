#pragma once
#include <iostream>
#include <sstream>

// TODO: проверить корректность методов
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
        };

        node *root = nullptr;

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

        bool is_red(node *z)
        {
            if (z == nullptr) {
                return false;
            }

            return z->color == node_color::red;
        }

        // TODO: НО КАК
        void insert_fixup(node *z)
        {
            if (z == nullptr)
                return;

            while (is_red(z->parent)) {
                if (z->parent == z->parent->parent->left_child) {
                    node *y = z->parent->parent->right_child;
                    if (is_red(y)) {
                        z->parent->color = node_color::black;
                        y->color = node_color::black;
                        z->parent->parent->color = node_color::red;
                        z = z->parent->parent;
                    } else {
                        if (z == z->parent->right_child) {
                            z = z->parent;
                            left_rotate(z);
                        }
                        // Может ли тут быть nullptr?
                        z->parent->color = node_color::black;
                        z->parent->parent->color = node_color::red;
                        right_rotate(z->parent->parent);
                    }
                } else {
                    node *y = z->parent->parent->left_child;
                    if (is_red(y)) {
                        z->parent->color = node_color::black;
                        y->color = node_color::black;
                        z->parent->parent->color = node_color::red;
                        z = z->parent->parent;
                    } else {
                        if (z == z->parent->left_child) {
                            z = z->parent;
                            right_rotate(z);
                        }
                        z->parent->color = node_color::black;
                        z->parent->parent->color = node_color::red;
                        left_rotate(z->parent->parent);
                    }
                }
            }
            root->color = node_color::black;
        }

        void remove_fixup(node *x, node *parent = nullptr)
        {
            node *current = x;
            node fake_current;
            if (x == nullptr) {
                fake_current.color = node_color::black;
                fake_current.parent = parent;
                current = &fake_current;
            }

            while (current != root && !is_red(current)) {
                // TODO: стереть правую ветку, добавить проверки на nullptr в левую,
                // переписать правую.

                if (current == current->parent->left_child) {
                    node *brother = current->parent->right_child;

                    // Случай 1
                    if (is_red(brother)) {
                        brother->color = node_color::black;
                        current->parent->color = node_color::red;
                        left_rotate(current->parent);
                        brother = current->parent->right_child;
                    }

                    // Случай 2
                    if (!is_red(brother->left_child) && !is_red(brother->right_child)) {
                        brother->color = node_color::red;
                        current = current->parent;
                    } else {
                        // Случай 3
                        if (!is_red(brother->right_child)) {
                            brother->left_child->color = node_color::black;
                            brother->color = node_color::red;
                            right_rotate(brother);
                            brother = current->parent->right_child;
                        }
                        // Случай 4
                        brother->color = current->parent->color;
                        current->parent->color = node_color::black;
                        brother->right_child->color = node_color::black;
                        left_rotate(current->parent);
                        current = root;
                    }
                } else {
                    node *brother = current->parent->left_child;

                    // Случай 1
                    if (is_red(brother)) {
                        brother->color = node_color::black;
                        current->parent->color = node_color::red;
                        right_rotate(current->parent);
                        brother = current->parent->left_child;
                    }

                    // Случай 2
                    if (!is_red(brother->right_child) && !is_red(brother->left_child)) {
                        brother->color = node_color::red;
                        current = current->parent;
                    } else {
                        // Случай 3
                        if (!is_red(brother->left_child)) {
                            brother->right_child->color = node_color::black;
                            brother->color = node_color::red;
                            left_rotate(brother);
                            brother = current->parent->left_child;
                        }
                        // Случай 4
                        brother->color = current->parent->color;
                        current->parent->color = node_color::black;
                        brother->left_child->color = node_color::black;
                        right_rotate(current->parent);
                        current = root;
                    }
                }
            }

            if (current != nullptr) {
                current->color = node_color::black;
            }
        }

    public:
        ~tree()
        {
        }

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

        // TODO: заменить node на что-то вроде итератора
        node *minimum(node *current = root)
        {
            // node *current = root;

            if (current == nullptr)
                return current;

            while (current->left_child != nullptr) {
                current = current->left_child;
            }

            return current;
        }

        // TODO: заменить node на что-то вроде итератора
        node *maximum(node *current = root)
        {
            // node *current = root;

            if (current == nullptr)
                return current;

            while (current->right_child != nullptr) {
                current = current->right_child;
            }

            return current;
        }

        // TODO: заменить node на что-то вроде итератора
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

        // TODO: заменить node на что-то вроде итератора
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

        // TODO: Заменить node на что-то вроде итератора
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
                    // current->parent = removable->parent;
                    // current->left_child = removable->left_child;
                    // current->right_child = removable->right_child;
                    current->color = removable->color;
                    current->key = removable->key;
                }
            }

            // Если узел чёрный, то могли быть нарушены КЧ-свойства
            // => необходимо их восстановить
            //
            // А если заменить на !is_red(removable)?
            if (removable != nullptr && removable->color == node_color::black) {
                remove_fixup(child);
            }

            return removable;
        }

        void traverse(node *current, int indent)
        {
            if (current == nullptr)
                return;

            std::cout << '\n';

            for (int i = 0; i < indent; ++i) {
                std::cout << '\t';
            }

            std::cout << "key:" << current->key << "; color: " << (current->color == node_color::black ? "black" : "red");

            traverse(current->left_child, indent + 1);
            traverse(current->right_child, indent + 1);
        }

        void traverse()
        {
            traverse(root, 0);
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

        std::string traverse_html()
        {
            return "<div class='tree'><ul>" + traverse_html(root) + "</ul></div>";
        }
    };
} // namespace rb_tree
