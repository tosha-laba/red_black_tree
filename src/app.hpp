#pragma once

#include "rb_tree.hpp"
#include <AppCore/App.h>
#include <AppCore/JSHelpers.h>
#include <AppCore/Overlay.h>
#include <AppCore/Window.h>
#include <ctime>
#include <fstream>

using namespace ultralight;

class TreeApp : public WindowListener, LoadListener
{
    RefPtr<App> m_app;
    RefPtr<Window> m_window;
    RefPtr<Overlay> m_overlay;

    const int app_width = 905;
    const int app_height = 768;

    rb_tree::tree<int> tree;

    std::ofstream log_file;

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

        log_file.open("rbt.log", std::ios_base::app);
        auto time = std::time(nullptr);
        log_file << "Session: " << std::ctime(&time) << '\n';
        log_file.flush();
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

            // Logging
            log_file << "Tree generation:\nKeys: ";

            for (unsigned int i = 0; i < array.length(); ++i) {
                int value = static_cast<int>(array[i].ToInteger());
                log_file << value << ' ';
            }

            tree.traverse(log_file);
            log_file << "\n\n";
            log_file.flush();
        }
    }

    void AddValue(const JSObject &thisObject, const JSArgs &args) {}

    void DeleteValue(const JSObject &thisObject, const JSArgs &args)
    {
        JSEval("informInputAboutError(DeleteKeyInput, deleteKeyIsCorrect());");

        auto isKeyCorrect = JSEval("deleteKeyIsCorrect();");

        if (isKeyCorrect.IsBoolean() && isKeyCorrect.ToBoolean()) {
            int value = static_cast<int>(JSEval("convertDeleteKeyToInt()").ToInteger());

            auto found = tree.find_weak(value);
            auto father = found != nullptr ? found->parent : nullptr;

            std::stringstream father_key;

            if (father != nullptr) {
                father_key << father->key;
            }

            delete tree.remove(father);

            std::string htmlTreeRepresentation = "SecondTree.innerHTML = \"" + tree.traverse_html() + "\";";
            JSEval(htmlTreeRepresentation.c_str());

            String fatherFound = father != nullptr ? "true" : "false";
            JSEval(JSString("informInputAboutGoodOperation(DeleteKeyInput,") + fatherFound + String(")"));

            // Logging
            log_file << "Removing:\n";
            log_file << "Child of removable element: ";
            log_file << value << "\n";

            log_file << "Removable element: ";
            if (father == nullptr) {
                log_file << "Not found\n\n";
            } else {
                log_file << father_key.str();
                log_file << "\nTree:";

                tree.traverse(log_file);
                log_file << "\n\n";
            }
            log_file.flush();
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