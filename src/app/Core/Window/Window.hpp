#pragma once

#include "pch.hpp"
#include "Event.hpp"


namespace window {
    struct Config {
        int width;
        int height;

        uint8_t samples = 0;
        bool resizable = false;  // This can be converted to a flag bit type thing when additional features are required.
    };


    namespace details {
        // Strategy pattern.
        // Why do we need this?
        //   . GLFW doesn't give good messages when calling a window method on nullptr. Should we care?
        //   . If we fix the underlying issue, we don't need this indirection.
        //     . Keep it but strip it out in release mode?
        //       . Might get it somewhere you don't expect.
        //   . Fine here because everything gets called at most a few times per frame.
        //   . Global "Window" object will initialize GLFW but can't create a window on its own.
        //      With this, any code that calls out to this uninitialized global will get a good message about it.
        //   . They might just use if(!window.created()) to avoid fixing the problem
        //      This can happen with or without this abstraction.
        //   . Tried this as a union where uninitialized gets instantiated by default. Compiler doesn't like it.
        //   . Code needs the window to be created.
        //     . With this, we get a message about it, then the actual code fails.
        //     . Undefined behavior territory. Code is made expecting a window to be there.
        //        The code might not actually fail.
        //       . We still get a message about it
        //         . Buried in a log somewhere
        //           . Do we just want errors instead of messages?
        //             . Errors will be disabled or turned into messages buried in a log somewhere.
        struct GLFWStateAbstract {
            virtual void set_user_pointer(void*) = 0;
            virtual void set_key_callback(GLFWkeyfun) = 0;
            virtual void set_cursor_callback(GLFWcursorposfun) = 0;
            virtual void set_mouse_callback(GLFWmousebuttonfun) = 0;
            virtual void set_text_callback(GLFWcharfun) = 0;

            virtual bool should_close() = 0;
            virtual void should_close(bool) = 0;
            virtual void swap_buffers() = 0;
            virtual void clear() = 0;
            virtual bool created() = 0;

            virtual ~GLFWStateAbstract() = default;
        };

        struct GLFWStateEmpty final : public GLFWStateAbstract {
            GLFWStateEmpty() = default;

            void set_user_pointer(void*) override;
            void set_key_callback(GLFWkeyfun) override;
            void set_cursor_callback(GLFWcursorposfun) override;
            void set_mouse_callback(GLFWmousebuttonfun) override;
            void set_text_callback(GLFWcharfun) override;

            bool should_close() override;
            void should_close(bool) override;
            void swap_buffers() override;
            void clear() override;
            bool created() override;
        };

        class GLFWState final : public GLFWStateAbstract {
            void destroy();
        public:
            GLFWState(const char* title, Config const&);

            void set_user_pointer(void*) override;
            void set_key_callback(GLFWkeyfun) override;
            void set_cursor_callback(GLFWcursorposfun) override;
            void set_mouse_callback(GLFWmousebuttonfun) override;
            void set_text_callback(GLFWcharfun) override;

            bool should_close() override;
            void should_close(bool) override;
            void swap_buffers() override;
            void clear() override;
            bool created() override;
        private:
            GLFWwindow* m_state;
        };
    }


    class Window {
    public:
        static Window &get();
    private:
        static void glfw_init();
        static Window* get_state(GLFWwindow*);
        static constexpr size_t DefaultEventStackCapacity = 8;

        Window();
    public:
        Window(Window const&) = delete;
        Window(Window&&) = delete;

        void create(const char* title, Config const&);
        bool created();

        bool should_close();
        void should_close(bool);
        void swap_buffers();
        void clear();
        void update();

        std::optional<Event> poll_event();
    private:
        // Fight the urge to optimize this.
        std::vector<Event> m_events {};
        std::unique_ptr<details::GLFWStateAbstract> m_state = std::make_unique<details::GLFWStateEmpty>();
    };
}
