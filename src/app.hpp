#pragma once

#include "rb_tree.hpp"
#include <AppCore/App.h>
#include <AppCore/JSHelpers.h>
#include <AppCore/Overlay.h>
#include <AppCore/Window.h>

using namespace ultralight;

/*
 * TODO:
 * 1. Пофиксить визуализацию дерева, чтобы не ломалось при кол-ве вершин > 30
 * 2. Сделать деструкторы для дерева и его узлов V
 * 3. Проверить корректность работы дерева, а именно:
 *      3.1: выяснить, почему при добавлении узлов, узлы равные корню могут попасть в левое поддерево V
 *      3.2: исправить "NPE" при удалении узлов. Пример данных: ввести 1-15, удалить узел 10 V ?
 *      3.3: поглядеть, могут ли удаляемые узлы содержать ненулевые указатели на потомков
 * 4. Добавить ветвь сборки, чтобы можно было удалять и отца корня (nullptr) если что. Не надо.
 * 5. Пофиксить split в input'aх, чтобы убирал пустые элементы. V
 * 6. Написать новый поиск, который будет учитывать нестрогие границы КЧД (Дерево 10 10 10 станет полным высоты 2).
 */
class TreeApp : public WindowListener, LoadListener
{
    RefPtr<App> m_app;
    RefPtr<Window> m_window;
    RefPtr<Overlay> m_overlay;

    const int app_width = 905;
    const int app_height = 768;

    rb_tree::tree<int> tree;

public:
    TreeApp()
    {
        m_app = App::Create();

        m_window = Window::Create(
            m_app->main_monitor(),
            app_width, app_height,
            false,
            kWindowFlags_Titled | kWindowFlags_Resizable | kWindowFlags_Maximizable);

        m_window->SetTitle("Red Black Tree");

        m_app->set_window(*m_window.get());

        m_overlay = Overlay::Create(
            *m_window.get(),
            m_window->width(),
            m_window->height(),
            0, 0);

        m_overlay->view()->set_load_listener(this);
        m_overlay->view()->LoadURL("file:///index.html");

        m_window->set_listener(this);
    }

    virtual ~TreeApp() {}

    virtual void OnClose() {}

    virtual void OnResize(int width, int height)
    {
        if (width != 0 && height != 0) {
            m_overlay->Resize(width, height);
        }
    }

    void MakeTree(const JSObject &thisObject, const JSArgs &args)
    {
        JSEval("informInputAboutError(InputKeysInput, inputKeysAreCorrect());");

        auto areKeysCorrect = JSEval("inputKeysAreCorrect();");

        if (areKeysCorrect.IsBoolean() && areKeysCorrect.ToBoolean()) {
            tree = rb_tree::tree<int>();

            auto array = JSEval("convertInputKeysToArray()").ToArray();
            for (unsigned int i = 0; i < array.length(); ++i) {
                int value = static_cast<int>(array[i].ToInteger());
                tree.insert(value);
            }

            auto htmlTreeRepresentation = String(tree.traverse_html().c_str());

            JSEval(JSString("FirstTree.innerHTML = \"") + htmlTreeRepresentation + JSString("\";"));
            JSEval(JSString("SecondTree.innerHTML = \"") + htmlTreeRepresentation + JSString("\";"));
            JSEval("informInputAboutGoodOperation(InputKeysInput, true);");
        }
    }

    void AddValue(const JSObject &thisObject, const JSArgs &args) {}

    void DeleteValue(const JSObject &thisObject, const JSArgs &args)
    {
        JSEval("informInputAboutError(DeleteKeyInput, deleteKeyIsCorrect());");

        auto isKeyCorrect = JSEval("deleteKeyIsCorrect();");

        if (isKeyCorrect.IsBoolean() && isKeyCorrect.ToBoolean()) {
            int value = static_cast<int>(JSEval("convertDeleteKeyToInt()").ToInteger());

            auto found = tree.find(value);
            auto father = found != nullptr ? found->parent : nullptr;

            tree.remove(father);

            std::string htmlTreeRepresentation = "SecondTree.innerHTML = \"" + tree.traverse_html() + "\";";
            JSEval(htmlTreeRepresentation.c_str());

            String fatherFound = father != nullptr ? "true" : "false";
            JSEval(JSString("informInputAboutGoodOperation(DeleteKeyInput,") + fatherFound + String(")"));
        }
    }

    void ResetState(const JSObject &thisObject, const JSArgs &args)
    {
        tree = rb_tree::tree<int>();
        JSEval("clearAll();");
    }

    virtual void OnDOMReady(ultralight::View *caller)
    {
        SetJSContext(caller->js_context());

        JSObject global = JSGlobalObject();

        // Callbacks
        global["MakeTree"] = BindJSCallback(&TreeApp::MakeTree);
        global["AddValue"] = BindJSCallback(&TreeApp::AddValue);
        global["DeleteValue"] = BindJSCallback(&TreeApp::DeleteValue);
        global["ResetState"] = BindJSCallback(&TreeApp::ResetState);

        // Global objects
        global["FirstTree"] = JSEval("document.getElementById('first_tree');");
        global["SecondTree"] = JSEval("document.getElementById('second_tree');");
        global["InputKeysInput"] = JSEval("document.getElementsByName('tree-values')[0];");
        global["AddKeyInput"] = JSEval("document.getElementsByName('tree-add-value')[0];");
        global["DeleteKeyInput"] = JSEval("document.getElementsByName('tree-delete-value')[0];");
    }

    void Run()
    {
        m_app->Run();
    }
};