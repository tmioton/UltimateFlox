// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

#include "pch.hpp"

// Material support later


class Object {
public:
    Object(size_t vertexCount, size_t indexCount, lwvl::PrimitiveMode);
    Object(Object&&) noexcept;

    Object& operator=(Object&&) noexcept;

    [[nodiscard]] const std::vector<float> &vertices() const;
    [[nodiscard]] std::vector<float> &vertices();
    [[nodiscard]] size_t vertexCount() const;

    [[nodiscard]] const std::vector<unsigned int> &indices() const;
    [[nodiscard]] std::vector<unsigned int> &indices();
    [[nodiscard]] size_t indexCount() const;

    [[nodiscard]] lwvl::PrimitiveMode drawMode() const;
private:
    size_t m_vertexCount;
    size_t m_indexCount;

    lwvl::PrimitiveMode m_drawMode;

    // Tie this to a common Object allocator
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
};

Object loadObject(
    const float *vertices, size_t vertexCount,
    const unsigned int *indices, size_t indexCount,
    lwvl::PrimitiveMode mode
);

void loadObject(
    std::unordered_map<std::string, Object>& objects,
    const std::string& objDescriptor
);
