#pragma once

#include "pch.hpp"

namespace lwvl {
    namespace details {
        enum class BufferTarget {
            Array = GL_ARRAY_BUFFER,
            Element = GL_ELEMENT_ARRAY_BUFFER,
            Texture = GL_TEXTURE_BUFFER,
            Uniform = GL_UNIFORM_BUFFER,
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
        };
    }

    enum class Usage {
        Static = GL_STATIC_DRAW,
        Dynamic = GL_DYNAMIC_DRAW,
        Stream = GL_STREAM_DRAW
    };

    template<details::BufferTarget target>
    class Buffer {
        class ID {
            static unsigned int reserve() {
                unsigned int tempID;
                glGenBuffers(1, &tempID);
                return tempID;
            }

        public:
            ~ID() {
                glDeleteBuffers(1, &bufferID);
            }

            explicit operator uint32_t() const {
                return bufferID;
            }

            const uint32_t bufferID = reserve();
        };

        // Offsite Data - to avoid copying buffers on the GPU for simple copies of this class.
        std::shared_ptr<Buffer::ID> m_offsite_id = std::make_shared<Buffer::ID>();

        // Local Data
        uint32_t m_id = static_cast<uint32_t>(*m_offsite_id);
        Usage m_usage = Usage::Dynamic;

    public:
        uint32_t id() {
            return m_id;
        }

        Buffer() = default;

        explicit Buffer(Usage usage) : m_usage(usage) {}

        Buffer(const Buffer &other) = default;

        Buffer &operator=(const Buffer &other) = default;

        Buffer(Buffer &&other) noexcept = default;

        Buffer &operator=(Buffer &&other) noexcept = default;

        template<typename T>
        void construct(const T *data, GLsizei count) {
            glBufferData(
                static_cast<GLenum>(target), sizeof(T) * count,
                data, static_cast<GLenum>(m_usage)
            );
        }

        template<class Iterator>
        void construct(Iterator first, Iterator last) {
            glBufferData(
                static_cast<GLenum>(target), sizeof(*first) * (last - first),
                &(*first), static_cast<GLenum>(m_usage)
            );
        }

        template<typename T>
        void update(const T *data, GLsizei count, GLsizei offsetCount = 0) {
            glBufferSubData(static_cast<GLenum>(target), offsetCount * sizeof(T), count * sizeof(T), data);
        }

        template<class Iterator>
        void update(Iterator first, Iterator last, GLsizei offsetCount = 0) {
            glBufferSubData(
                static_cast<GLenum>(target), offsetCount * sizeof(*first), sizeof(*first) * (last - first), &(*first));
        }

        void usage(Usage usage) { m_usage = usage; }

        Usage usage() { return m_usage; }

        // These operations should not be const because they modify GL state.
        void bind() {
            glBindBuffer(
                static_cast<GLenum>(target),
                m_id
            );
        }

        static void clear() {
            glBindBuffer(static_cast<GLenum>(target), 0);
        }
    };


    typedef Buffer<details::BufferTarget::Array> ArrayBuffer;
    typedef Buffer<details::BufferTarget::Element> ElementBuffer;
    typedef Buffer<details::BufferTarget::Texture> TextureBuffer;
    typedef Buffer<details::BufferTarget::Uniform> UniformBuffer;
    typedef Buffer<details::BufferTarget::ShaderStorage> ShaderStorageBuffer;
}
