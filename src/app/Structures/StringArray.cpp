#include "pch.hpp"
#include "StringArray.hpp"


// ****** SegmentGroup ******
const char *StringArray::SegmentGroup::c_str(size_t index) {
    return data.at(index).data();
}


// ****** StringInfo ******
// TODO: Check if divide does give mod for free.
StringArray::StringInfo::StringInfo(const int32_t s, const size_t l) :
    group(s / static_cast<int32_t>(StringArray::SegmentGroup::Length)),
    start(s % static_cast<int32_t>(StringArray::SegmentGroup::Length)),
    length(l)
{}

StringArray::StringInfo::StringInfo(const int32_t g, const int32_t s, const size_t l) :
    group(g), start(s), length(l)
{}


// ****** StringArray ******
std::string_view StringArray::string(const StringArray::StringInfo &info) const {
    return {m_segment_groups.at(info.group)->c_str(info.start), static_cast<size_t>(info.length + 1)};
}

void StringArray::check_size(const size_t new_segment_count) {

}

StringArray::StringArray() {
    m_segment_groups.emplace_back();
    m_string_info.reserve(128);
}

void StringArray::clear() {
    m_segment_groups.clear();
    m_segment_groups.emplace_back();
    m_string_info.clear();
}

void StringArray::push_back(const char *str, const size_t max) {
    const size_t new_index = m_segments.size();
    const size_t length = strnlen_s(str, max) + 1;

    // Right now this can potentially cause the null terminator to have its own segment.
    // It's fine while we prove we can sometimes leave off the null terminator.
    const size_t segments = (length / SegmentSize) + (length % SegmentSize > 0);

    // Grow the array preemptively if needed. This prevents two growths back to back.
    check_size(segments);

    for (size_t i = 0; i < segments; ++i) { m_segments.emplace_back(); }
    strcpy_s(m_segments.at(new_index).data(), SegmentSize * segments, str);
    m_string_info.emplace_back(new_index, length - 1);
}

std::string_view StringArray::at(const size_t index) const {
    const StringInfo &info {m_string_info.at(index)};
    return {m_segment_groups.at(info.group)->c_str(info.start), info.length + 1};
}

std::string StringArray::copy(const size_t index) const {
    const StringInfo &info {m_string_info.at(index)};
    return {m_segment_groups.at(info.group)->c_str(info.start), info.length + 1};
}
