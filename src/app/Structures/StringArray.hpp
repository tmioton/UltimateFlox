#pragma once

#include "pch.hpp"

// How do we deal with extra capacity?

class StringArray {
    // We have an array of segments, but one string can be multiple segments.
    // String segments are always contiguous. Any editing beyond existing segments creates a new chain.
    static constexpr size_t SegmentSize = 64;  // bytes
    typedef std::array<char, SegmentSize> Segment;

    struct SegmentGroup {
        static constexpr size_t Length = 128;
        std::array<Segment, Length> data {};
        unsigned int used {0};

        const char* c_str(size_t index);
    };

    struct StringInfo {
        // int32_t is fine, but everything works in these types, and they're fine too.
        //   Doubt the cache performance of this means anything.
        int32_t group {0};
        int32_t start {0};
        size_t length {0};  // Length of string without \0. length / 64 is number of segments.

        StringInfo() = default;
        StringInfo(int32_t index, size_t length);
        StringInfo(int32_t group, int32_t start, size_t length);
    };

    [[nodiscard]] std::string_view string(const StringInfo &info) const;
    void check_size(size_t new_segment_count);
public:
    StringArray();

    inline void clear();
    void push_back(const char *str, size_t max);
    [[nodiscard]] std::string_view at(size_t index) const;
    [[nodiscard]] std::string copy(size_t index) const;
private:
    // vector<Segment> forces a copy of kilobytes of data.
    std::vector<std::unique_ptr<SegmentGroup>> m_segment_groups {};
    //std::vector<Segment> m_segments {};
    std::vector<StringInfo> m_string_info {};
};
