#include "pch.hpp"
#include "Event.hpp"

window::Event::Event() : type(Type::KeyPress) {}

window::Event::Event(Type type, AnonymousEvent event) : type(type), event(event) {}
