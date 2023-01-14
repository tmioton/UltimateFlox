#pragma once

#include "pch.hpp"
#include "Event.hpp"


extern constexpr size_t EventStackCapacity = 8;


namespace window {
    struct Config {
        int width;
        int height;

        uint8_t samples = 1;
        bool resizable = false;  // This can be converted to a flag bit type thing when additional features are required.
    };


    namespace details {
        // Strategy pattern.
        struct GLFWState {
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

            virtual ~GLFWState() = default;
        };

        struct EmptyGLFWState final : public GLFWState {
            EmptyGLFWState() = default;

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

        class CreatedGLFWState final : public GLFWState {
            void destroy();
        public:
            CreatedGLFWState(const char* title, Config const&);

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
        std::unique_ptr<details::GLFWState> m_state = std::make_unique<details::EmptyGLFWState>();
    };
}
