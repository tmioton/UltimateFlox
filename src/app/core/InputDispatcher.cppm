module;
#include "pch.hpp"
export module InputDispatcher;

import Event;


namespace core {
    export struct InputLayer {
        virtual ~InputLayer() = default;
        virtual bool handle_event(input::Event const &event) { return false; };
    };

    export class InputDispatcher {
        std::vector<InputLayer *> layer_stack;

        void dispatch_event(input::Event const &event) const {
            for (const auto layer : layer_stack) {
                if (layer->handle_event(event)) break;
            }
        }

    public:
        void trigger(input::MouseButtonEvent const &event) const noexcept {
            dispatch_event(input::Event{.type = input::Event::Type::MouseButton, .button = event});
        }

        void trigger(input::CursorEvent const &event) const noexcept {
            dispatch_event(input::Event{.type = input::Event::Type::Cursor, .cursor = event});
        }

        void trigger(input::ScrollEvent const &event) const noexcept {
            dispatch_event(input::Event{.type = input::Event::Type::Scroll, .scroll = event});
        }

        void trigger(input::KeyEvent const &event) const noexcept {
            dispatch_event(input::Event{.type = input::Event::Type::Key, .key = event});
        }

        void trigger(input::TextEvent const &event) const noexcept {
            dispatch_event(input::Event{.type = input::Event::Type::Text, .text = event});
        }

        void trigger(input::ModifiedTextEvent const &event) const noexcept {
            dispatch_event(input::Event{.type = input::Event::Type::ModifiedText, .modified_text = event});
        }

        void push_layer(InputLayer *layer) noexcept {
            layer_stack.push_back(layer);
        }

        void pop_layer() noexcept {
            layer_stack.pop_back();
        }

        void clear() noexcept {
            layer_stack.clear();
        }
    };
} // namespace core
