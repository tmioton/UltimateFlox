#pragma once
#include "pch.hpp"

template<typename T>
class RawArray {
    T* m_data;
    std::size_t m_size;
public:
    RawArray(T* data, std::size_t size) : m_data(data), m_size(size) {}

    T* begin() {
        return m_data;
    }

    T* end() {
        return m_data + m_size;
    }
};
