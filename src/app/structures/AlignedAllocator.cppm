module;
#include "pch.hpp"
export module AlignedAllocator;

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


export template <typename T, std::size_t Alignment = alignof(T)>
class AlignedAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    static constexpr std::size_t alignment = Alignment;

    template <class U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };

    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;

    constexpr AlignedAllocator() noexcept = default;

    template <typename U>
    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    // Required for C++11 and later
    constexpr bool operator==(const AlignedAllocator&) const noexcept {
        return true;
    }

    constexpr bool operator!=(const AlignedAllocator &other) const noexcept { return !(*this == other); }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    [[nodiscard]] T *allocate(const std::size_t n) {
        if (n == 0) {
            return nullptr;
        }

        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_array_new_length();
        }

        void *p = aligned_alloc(Alignment, n * sizeof(T));
        if (p == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<T *>(p);
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void deallocate(T* p, std::size_t n) noexcept {
        return aligned_free(p);
    }
};


namespace test {
    export template <typename Allocator>
    [[nodiscard]] bool test_aligned_allocation(Allocator alloc, std::size_t num_elements) {
        using T = typename std::allocator_traits<Allocator>::value_type;
        const std::size_t alignment = Allocator::alignment;

        std::unique_ptr<T, std::function<void(T*)>> ptr(
            alloc.allocate(num_elements),
            [&](T* p) { alloc.deallocate(p, num_elements); }
        );

        if (const uintptr_t address = reinterpret_cast<uintptr_t>(ptr.get()); address % alignment == 0) {
            return true;
        }
        return false;
    }
}
