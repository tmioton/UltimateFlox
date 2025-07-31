module;
#include "pch.hpp"
export module DoubleBuffer;


inline void* aligned_alloc(size_t alignment, size_t size) {
#ifdef WIN32
    return _aligned_malloc(size, alignment);
#else
    return std::aligned_alloc(alignment, size);
#endif
}

inline void aligned_free(void* block) {
#ifdef WIN32
    return _aligned_free(block);
#else
    return std::free(block);
#endif
}


export template<typename T>
class DoubleBuffer {
public:
    explicit DoubleBuffer(const size_t initial_count) :
            m_reserved(initial_count), m_count(initial_count), m_primary(nullptr), m_secondary(nullptr) {
        const size_t allocation_size = initial_count * sizeof(T);
        if (allocation_size == 0) {
            return;
        }

        void* p_primary = aligned_alloc(64, allocation_size);
        if (!p_primary) {
            throw std::bad_alloc();
        }
        m_primary = static_cast<T*>(p_primary);

        void* p_secondary = aligned_alloc(64, allocation_size);
        if (!p_secondary) {
            aligned_free(p_primary);
            throw std::bad_alloc();
        }
        m_secondary = static_cast<T*>(p_secondary);
    }

    ~DoubleBuffer() {
        aligned_free(m_primary);
        aligned_free(m_secondary);
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
            const size_t alloc_size = new_count * sizeof(T);
            void *p_primary = aligned_alloc(64, alloc_size);
            if (!p_primary) {
                throw std::bad_alloc();
            }
            T *new_primary = static_cast<T*>(p_primary);

            void *p_secondary = aligned_alloc(64, alloc_size);
            if (!p_secondary) {
                aligned_free(p_primary);
                throw std::bad_alloc();
            }
            T *new_secondary = static_cast<T*>(p_secondary);

            // Could really just copy m_secondary to both new arrays,
            // but it doesn't hurt to support extra functionality.
            std::copy(m_primary, m_primary + m_count, new_primary);
            std::copy(m_secondary, m_secondary + m_count, new_secondary);

            aligned_free(m_primary);
            aligned_free(m_secondary);

            m_primary = new_primary;
            m_secondary = new_secondary;
            m_reserved = new_count;
        }
        m_count = new_count;
    }

    [[nodiscard]] size_t count() const {
        return m_count;
    }

    size_t m_reserved;
    size_t m_count;

    T *m_primary;
    T *m_secondary;
};
