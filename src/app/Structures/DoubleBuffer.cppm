module;
#include "pch.hpp"
export module DoubleBuffer;


export template<typename T>
class DoubleBuffer {
public:
    explicit DoubleBuffer(const size_t initial_count) :
        m_reserved(initial_count), m_count(initial_count),
        m_primary(new T[initial_count]),
        m_secondary(new T[initial_count])
    {}

    ~DoubleBuffer() {
        delete[] m_primary;
        delete[] m_secondary;
    }

    void flip() {
        std::copy(m_primary, m_primary + m_count, m_secondary);
    }

    T const *read() const {
        return m_secondary;
    }

    T *write() {
        return m_primary;
    }

    void resize(const size_t new_count) {
        if (new_count > m_reserved) {
            T *new_primary = new T[new_count];
            T *new_secondary = new T[new_count];

            // Could really just copy m_secondary to both new arrays,
            // but it doesn't hurt to support extra functionality.
            std::copy(m_primary, m_primary + m_count, new_primary);
            std::copy(m_secondary, m_secondary + m_count, new_secondary);

            delete[] m_primary;
            delete[] m_secondary;

            m_primary = new_primary;
            m_secondary = new_secondary;
            m_reserved = new_count;
        }
        m_count = new_count;
    }

    [[nodiscard]] size_t count() const {
        return m_count;
    }

public:
    size_t m_reserved;
    size_t m_count;

    T *m_primary;
    T *m_secondary;
};

