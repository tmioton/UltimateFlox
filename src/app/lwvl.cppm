module;
#include "pch.hpp"
export module lwvl;

namespace lwvl {
    enum class BufferType : uint8_t {
        Array,
        AtomicCounter,
        CopyRead,
        CopyWrite,
        DispatchIndirect,
        DrawIndirect,
        Element,
        PixelPack,
        PixelUnpack,
        Query,
        ShaderStorage,
        Texture,
        TransformFeedback,
        Uniform
    };

    GLenum to_GLenum(const BufferType type) {
        switch (type) {
            case BufferType::Array: return GL_ARRAY_BUFFER;
            case BufferType::AtomicCounter: return GL_ATOMIC_COUNTER_BUFFER;
            case BufferType::CopyRead: return GL_COPY_READ_BUFFER;
            case BufferType::CopyWrite: return GL_COPY_WRITE_BUFFER;
            case BufferType::DispatchIndirect: return GL_DISPATCH_INDIRECT_BUFFER;
            case BufferType::DrawIndirect: return GL_DRAW_INDIRECT_BUFFER;
            case BufferType::Element: return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::PixelPack: return GL_PIXEL_PACK_BUFFER;
            case BufferType::PixelUnpack: return GL_PIXEL_UNPACK_BUFFER;
            case BufferType::Query: return GL_QUERY_BUFFER;
            case BufferType::ShaderStorage: return GL_SHADER_STORAGE_BUFFER;
            case BufferType::Texture: return GL_TEXTURE_BUFFER;
            case BufferType::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
            case BufferType::Uniform: return GL_UNIFORM_BUFFER;
            default: std::unreachable();
        }
    }

    enum class BufferTarget : uint8_t {
        AtomicCounter,
        TransformFeedback,
        Uniform,
        ShaderStorage
    };

    GLenum to_GLenum(const BufferTarget target) {
        switch (target) {
            case BufferTarget::AtomicCounter: return GL_ATOMIC_COUNTER_BUFFER;
            case BufferTarget::TransformFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
            case BufferTarget::Uniform: return GL_UNIFORM_BUFFER;
            case BufferTarget::ShaderStorage: return GL_SHADER_STORAGE_BUFFER;
            default: std::unreachable();
        }
    }

    export enum class MapAccess : uint8_t {
        Read, Write, Both
    };

    GLenum to_GLenum(const MapAccess access) {
        switch (access) {
            case MapAccess::Read: return GL_READ_ONLY;
            case MapAccess::Write: return GL_WRITE_ONLY;
            case MapAccess::Both: return GL_READ_WRITE;
            default: std::unreachable();
        }
    }

    export enum class ByteFormat : uint8_t {
        UnsignedByte, Byte,
        UnsignedShort, Short,
        UnsignedInt, Int,
        HalfFloat, Float, Fixed, Double,
        Int_2_10_10_10,
        UnsignedInt_2_10_10_10,
        UnsignedInt_10F_11F_11F
    };

    GLenum to_GLenum(const ByteFormat format) {
        switch (format) {
            case ByteFormat::UnsignedByte: return GL_UNSIGNED_BYTE;
            case ByteFormat::Byte: return GL_BYTE;
            case ByteFormat::UnsignedShort: return GL_UNSIGNED_SHORT;
            case ByteFormat::Short: return GL_SHORT;
            case ByteFormat::UnsignedInt: return GL_UNSIGNED_INT;
            case ByteFormat::Int: return GL_INT;
            case ByteFormat::HalfFloat: return GL_HALF_FLOAT;
            case ByteFormat::Float: return GL_FLOAT;
            case ByteFormat::Fixed: return GL_FIXED;
            case ByteFormat::Double: return GL_DOUBLE;
            case ByteFormat::Int_2_10_10_10: return GL_INT_2_10_10_10_REV;
            case ByteFormat::UnsignedInt_2_10_10_10: return GL_UNSIGNED_INT_2_10_10_10_REV;
            case ByteFormat::UnsignedInt_10F_11F_11F: return GL_UNSIGNED_INT_10F_11F_11F_REV;

            default: std::unreachable();
        }
    }

    namespace bits {
        export constexpr GLbitfield None = 0;

        export typedef enum GLbitfield {
            Dynamic       = GL_DYNAMIC_STORAGE_BIT,
            MapRead       = GL_MAP_READ_BIT,
            MapWrite      = GL_MAP_WRITE_BIT,
            MapPersistent = GL_MAP_PERSISTENT_BIT,
            MapCoherent   = GL_MAP_COHERENT_BIT,
            Client        = GL_CLIENT_STORAGE_BIT
        } UsageBits;
    }

    export class Buffer {
        static GLuint reserve() {
            GLuint temp;
            glCreateBuffers(1, &temp);
            return temp;
        }

    public:
        const GLuint id;

        Buffer() noexcept : id(reserve()) {}

        ~Buffer() {
            glDeleteBuffers(1, &id);
        }

        Buffer(const Buffer &)            = delete;
        Buffer& operator=(const Buffer &) = delete;

        Buffer(Buffer &&)            = delete;
        Buffer& operator=(Buffer &&) = delete;

        template <typename T>
        void store(const std::span<T> data, const GLbitfield usage = bits::None) const noexcept {
            glNamedBufferStorage(id, data.size_bytes(), data.data(), usage);
        }

        template <std::contiguous_iterator Iterator>
        void store(const Iterator begin, const Iterator end, const GLbitfield usage = bits::None) const noexcept {
            glNamedBufferStorage(id, sizeof(*begin) * (end - begin), &(*begin), usage);
        }

        template <typename T>
        void allocate(const std::size_t count, const GLbitfield usage = bits::None) const noexcept {
            glNamedBufferStorage(id, count * sizeof(T), nullptr, usage | bits::Dynamic);
        }

        template <typename T>
        void update(const std::span<T> data, const GLintptr offset = 0) const noexcept {
            glNamedBufferSubData(id, offset, data.size_bytes(), data.data());
        }

        template <std::contiguous_iterator Iterator>
        void update(const Iterator begin, const Iterator end, const GLintptr offset = 0) const noexcept {
            glNamedBufferSubData(id, offset, sizeof(*begin) * (end - begin), &(*begin));
        }

        template <typename T>
        T* map(const MapAccess access) const noexcept {
            return static_cast<T*>(glMapNamedBuffer(id, to_GLenum(access)));
        }

        void unmap() const noexcept {
            glUnmapNamedBuffer(id);
        }

        using Type = BufferType;
        using Target = BufferTarget;

        void bind(const Type type) const noexcept {
            glBindBuffer(to_GLenum(type), id);
        }

        void bind_base(const Target target, const GLuint binding) const noexcept {
            glBindBufferBase(to_GLenum(target), binding, id);
        }
    };

    export class VertexArray {
        static GLuint reserve() {
            GLuint temp;
            glCreateVertexArrays(1, &temp);
            return temp;
        }

        void attribute_format(
            const GLuint     attribute,
            const GLint      dimensions,
            const GLuint     offset,
            const ByteFormat format,
            const bool       normalized
        ) const noexcept {
            switch (format) {
                case ByteFormat::Double: {
                    glVertexArrayAttribLFormat(id, attribute, dimensions, to_GLenum(format), offset);
                    break;
                }
                case ByteFormat::Byte:
                case ByteFormat::UnsignedByte:
                case ByteFormat::Short:
                case ByteFormat::UnsignedShort:
                case ByteFormat::Int:
                case ByteFormat::UnsignedInt:
                case ByteFormat::Int_2_10_10_10:
                case ByteFormat::UnsignedInt_2_10_10_10: {
                    glVertexArrayAttribIFormat(id, attribute, dimensions, to_GLenum(format), offset);
                    break;
                }
                case ByteFormat::Float:
                case ByteFormat::HalfFloat:
                case ByteFormat::Fixed:
                case ByteFormat::UnsignedInt_10F_11F_11F:
                default: {
                    glVertexArrayAttribFormat(id, attribute, dimensions, to_GLenum(format), normalized, offset);
                    break;
                }
            }
        }

    public:
        const GLuint id;

        VertexArray() noexcept : id(reserve()) {}
        VertexArray(const VertexArray &)            = delete;
        VertexArray& operator=(const VertexArray &) = delete;
        VertexArray(VertexArray &&)                 = delete;
        VertexArray& operator=(VertexArray &&)      = delete;

        ~VertexArray() noexcept {
            glDeleteVertexArrays(1, &id);
        }

        void attribute(
            const GLuint     attribute,
            const GLint      dimensions,
            const GLuint     offset,
            const ByteFormat format     = ByteFormat::Float,
            const bool       normalized = false
        ) const noexcept {
            glEnableVertexArrayAttrib(id, attribute);
            attribute_format(attribute, dimensions, offset, format, normalized);
        }

        void attribute(
            const GLuint     binding,
            const GLuint     attribute,
            const GLint      dimensions,
            const GLuint     offset,
            const ByteFormat format     = ByteFormat::Float,
            const bool       normalized = false
        ) const noexcept {
            glEnableVertexArrayAttrib(id, attribute);
            attribute_format(attribute, dimensions, offset, format, normalized);
            glVertexArrayAttribBinding(id, attribute, binding);
        }

        void enable_attribute(const GLuint attribute) const noexcept {
            glEnableVertexArrayAttrib(id, attribute);
        }

        void disable_attribute(const GLuint attribute) const noexcept {
            glDisableVertexArrayAttrib(id, attribute);
        }

        void vertex_buffer(
            const Buffer & buffer,
            const GLuint   binding,
            const GLintptr offset,
            const GLsizei  stride
        ) const noexcept {
            glVertexArrayVertexBuffer(id, binding, buffer.id, offset, stride);
        }

        template <typename T>
        void vertex_buffer(const Buffer &buffer, const GLuint binding, const GLintptr offset) const noexcept {
            vertex_buffer(buffer, binding, offset, sizeof(T));
        }

        void divisor(const GLuint binding, const GLuint divisor) const noexcept {
            glVertexArrayBindingDivisor(id, binding, divisor);
        }

        void element_buffer(const Buffer &buffer) const noexcept {
            glVertexArrayElementBuffer(id, buffer.id);
        }

        void bind() const noexcept {
            glBindVertexArray(id);
        }
    };

    enum class ShaderType : uint8_t {
        Vertex, Fragment, Geometry, Compute,
        TessControl, TessEval
    };

    std::string_view to_str(const ShaderType type) {
        switch (type) {
            case ShaderType::Vertex: return "vertex";
            case ShaderType::Fragment: return "fragment";
            case ShaderType::Geometry: return "geometry";
            case ShaderType::Compute: return "compute";
            case ShaderType::TessControl: return "tessellation control";
            case ShaderType::TessEval: return "tessellation evaluation";
            default: std::unreachable();
        }
    }

    GLenum to_GLenum(const ShaderType type) {
        switch (type) {
            case ShaderType::Vertex: return GL_VERTEX_SHADER;
            case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
            case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
            case ShaderType::Compute: return GL_COMPUTE_SHADER;
            case ShaderType::TessControl: return GL_TESS_CONTROL_SHADER;
            case ShaderType::TessEval: return GL_TESS_EVALUATION_SHADER;
            default: std::unreachable();
        }
    }

    export class Shader {
    public:
        using Type = ShaderType;

        const GLuint id;
        const Type   type;

        explicit Shader(const Type shader_type) : id(glCreateShader(to_GLenum(shader_type))), type(shader_type) {}
        Shader(const Shader &)            = delete;
        Shader& operator=(const Shader &) = delete;
        Shader(Shader &&)                 = delete;
        Shader& operator=(Shader &&)      = delete;

        bool compile(const std::string_view source) const noexcept { // NOLINT(*-use-nodiscard)
            const GLchar *src = source.data();
            const auto    len = static_cast<GLsizei>(source.size());

            glShaderSource(id, 1, &src, &len);
            glCompileShader(id);

            int result;
            glGetShaderiv(id, GL_COMPILE_STATUS, &result);
            return result != GL_FALSE;
        }

        [[nodiscard]] GLsizei info_log_length() const noexcept {
            GLint length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            return length;
        }

        GLsizei info_log(std::span<char> buffer) const noexcept { // NOLINT(*-use-nodiscard)
            GLsizei length = 0;
            glGetShaderInfoLog(id, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
            return length;
        }

        std::string info_log_str() const noexcept {
            std::string buffer(info_log_length(), '\0');
            info_log(std::span{buffer});
            return buffer;
        }

        [[nodiscard]] std::string_view type_str() const noexcept {
            return to_str(type);
        }
    };

    export class Program;

    export struct Uniform {
        GLint location;

        [[nodiscard]] bool valid() const noexcept {
            return location > -1;
        }

        void set_float(const Program &program, float x) const noexcept;
        void set_fvec2(const Program &program, float x, float y) const noexcept;
        void set_fvec3(const Program &program, float x, float y, float z) const noexcept;
        void set_fvec4(const Program &program, float x, float y, float z, float w) const noexcept;
        void set_int(const Program &program, int x) const noexcept;
        void set_ivec2(const Program &program, int x, int y) const noexcept;
        void set_ivec3(const Program &program, int x, int y, int z) const noexcept;
        void set_ivec4(const Program &program, int x, int y, int z, int w) const noexcept;
        void set_uint(const Program &program, unsigned int x) const noexcept;
        void set_uvec2(const Program &program, unsigned int x, unsigned int y) const noexcept;
        void set_uvec3(const Program &program, unsigned int x, unsigned int y, unsigned int z) const noexcept;
        void set_uvec4(
            const Program &program, unsigned int x, unsigned int y, unsigned int z, unsigned int w
        ) const noexcept;
        void set_float_array(const Program &program, std::span<float> data) const noexcept;
        void set_fvec2_array(const Program &program, std::span<glm::fvec2> data) const noexcept;
        void set_fvec3_array(const Program &program, std::span<glm::fvec3> data) const noexcept;
        void set_fvec4_array(const Program &program, std::span<glm::fvec4> data) const noexcept;
        void set_int_array(const Program &program, std::span<int> data) const noexcept;
        void set_ivec2_array(const Program &program, std::span<glm::ivec2> data) const noexcept;
        void set_ivec3_array(const Program &program, std::span<glm::ivec3> data) const noexcept;
        void set_ivec4_array(const Program &program, std::span<glm::ivec4> data) const noexcept;
        void set_uint_array(const Program &program, std::span<unsigned int> data) const noexcept;
        void set_uvec2_array(const Program &program, std::span<glm::uvec2> data) const noexcept;
        void set_uvec3_array(const Program &program, std::span<glm::uvec3> data) const noexcept;
        void set_uvec4_array(const Program &program, std::span<glm::uvec4> data) const noexcept;
        void set_mat2x2(const Program &program, glm::mat2x2 matrix, bool transpose = false) const noexcept;
        void set_mat3x3(const Program &program, glm::mat3x3 matrix, bool transpose = false) const noexcept;
        void set_mat4x4(const Program &program, glm::mat4x4 matrix, bool transpose = false) const noexcept;
        void set_mat2x2_array(
            const Program &program, std::span<glm::mat2x2> data, bool transpose = false
        ) const noexcept;
        void set_mat3x3_array(
            const Program &program, std::span<glm::mat3x3> data, bool transpose = false
        ) const noexcept;
        void set_mat4x4_array(
            const Program &program, std::span<glm::mat4x4> data, bool transpose = false
        ) const noexcept;
    };

    export enum class ProgramCompilationResult {
        Success, LinkingFailure, ValidationFailure
    };

    class Program {
    public:
        const GLuint id;

        Program() : id(glCreateProgram()) {}
        Program(const Program &)            = delete;
        Program& operator=(const Program &) = delete;
        Program(Program &&)                 = delete;
        Program& operator=(Program &&)      = delete;

        ~Program() {
            glDeleteProgram(id);
        }

        void attach_shader(const Shader &shader) const noexcept {
            glAttachShader(id, shader.id);
        }

        void detach_shader(const Shader &shader) const noexcept {
            glDetachShader(id, shader.id);
        }

        bool link() const noexcept { // NOLINT(*-use-nodiscard)
            glLinkProgram(id);
            int result;
            glGetProgramiv(id, GL_LINK_STATUS, &result);
            return result != GL_FALSE;
        }

        bool validate() const noexcept { // NOLINT(*-use-nodiscard)
            glValidateProgram(id);
            int result;
            glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
            return result != GL_FALSE;
        }

        [[nodiscard]] GLsizei info_log_length() const noexcept {
            GLsizei length;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
            return length;
        }

        GLsizei info_log(std::span<char> buffer) const noexcept { // NOLINT(*-use-nodiscard)
            GLsizei length = 0;
            glGetProgramInfoLog(id, static_cast<GLsizei>(buffer.size()), &length, buffer.data());
            return length;
        }

        std::string info_log_str() const noexcept {
            std::string buffer(info_log_length(), '\0');
            info_log(std::span{buffer});
            return buffer;
        }

        Uniform uniform(const std::string_view name) const noexcept {
            const GLint location = glGetUniformLocation(id, name.data());
            return {location};
        }

        void uniform_block_binding(const std::string_view name, const GLuint binding) const noexcept {
            const GLint location = glGetUniformBlockIndex(id, name.data());
            glUniformBlockBinding(id, location, binding);
        }

        void bind() const noexcept {
            glUseProgram(id);
        }

        ProgramCompilationResult compile(const std::span<const Shader * const> shaders, std::span<char> error_buffer) const noexcept {
            for (const auto shader : shaders) {
                attach_shader(*shader);
            }

            if (!link()) {
                return ProgramCompilationResult::LinkingFailure;
            }

            if (!validate()) {
                return ProgramCompilationResult::ValidationFailure;
            }

            for (const auto shader : shaders) {
                detach_shader(*shader);
            }

            return ProgramCompilationResult::Success;
        }
    };

    void Uniform::set_float(const Program &program, const float x) const noexcept {
        glProgramUniform1f(program.id, location, x);
    }

    void Uniform::set_fvec2(const Program &program, const float x, const float y) const noexcept {
        glProgramUniform2f(program.id, location, x, y);
    }

    void Uniform::set_fvec3(const Program &program, const float x, const float y, const float z) const noexcept {
        glProgramUniform3f(program.id, location, x, y, z);
    }

    void Uniform::set_fvec4(
        const Program &program, const float x, const float y, const float z, const float w
    ) const noexcept {
        glProgramUniform4f(program.id, location, x, y, z, w);
    }

    void Uniform::set_int(const Program &program, const int x) const noexcept {
        glProgramUniform1i(program.id, location, x);
    }

    void Uniform::set_ivec2(const Program &program, const int x, const int y) const noexcept {
        glProgramUniform2i(program.id, location, x, y);
    }

    void Uniform::set_ivec3(const Program &program, const int x, const int y, const int z) const noexcept {
        glProgramUniform3i(program.id, location, x, y, z);
    }

    void Uniform::set_ivec4(const Program &program, const int x, const int y, const int z, const int w) const noexcept {
        glProgramUniform4i(program.id, location, x, y, z, w);
    }

    void Uniform::set_uint(const Program &program, const unsigned int x) const noexcept {
        glProgramUniform1ui(program.id, location, x);
    }

    void Uniform::set_uvec2(const Program &program, const unsigned int x, const unsigned int y) const noexcept {
        glProgramUniform2ui(program.id, location, x, y);
    }

    void Uniform::set_uvec3(
        const Program &program, const unsigned int x, const unsigned int y, const unsigned int z
    ) const noexcept {
        glProgramUniform3ui(program.id, location, x, y, z);
    }

    void Uniform::set_uvec4(
        const Program &program, const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int w
    ) const noexcept {
        glProgramUniform4ui(program.id, location, x, y, z, w);
    }

    void Uniform::set_float_array(const Program &program, const std::span<float> data) const noexcept {
        glProgramUniform1fv(program.id, location, static_cast<GLsizei>(data.size()), data.data());
    }

    void Uniform::set_fvec2_array(const Program &program, const std::span<glm::fvec2> data) const noexcept {
        glProgramUniform2fv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_fvec3_array(const Program &program, const std::span<glm::fvec3> data) const noexcept {
        glProgramUniform3fv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_fvec4_array(const Program &program, const std::span<glm::fvec4> data) const noexcept {
        glProgramUniform4fv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_int_array(const Program &program, const std::span<int> data) const noexcept {
        glProgramUniform1iv(program.id, location, static_cast<GLsizei>(data.size()), data.data());
    }

    void Uniform::set_ivec2_array(const Program &program, const std::span<glm::ivec2> data) const noexcept {
        glProgramUniform2iv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_ivec3_array(const Program &program, const std::span<glm::ivec3> data) const noexcept {
        glProgramUniform3iv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_ivec4_array(const Program &program, const std::span<glm::ivec4> data) const noexcept {
        glProgramUniform4iv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_uint_array(const Program &program, const std::span<unsigned int> data) const noexcept {
        glProgramUniform1uiv(program.id, location, static_cast<GLsizei>(data.size()), data.data());
    }

    void Uniform::set_uvec2_array(const Program &program, const std::span<glm::uvec2> data) const noexcept {
        glProgramUniform2uiv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_uvec3_array(const Program &program, const std::span<glm::uvec3> data) const noexcept {
        glProgramUniform3uiv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_uvec4_array(const Program &program, const std::span<glm::uvec4> data) const noexcept {
        glProgramUniform4uiv(program.id, location, static_cast<GLsizei>(data.size()), glm::value_ptr(data.front()));
    }

    void Uniform::set_mat2x2(
        const Program &program, const glm::mat2x2 matrix, const bool transpose
    ) const noexcept {
        glProgramUniformMatrix2fv(program.id, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    }

    void Uniform::set_mat3x3(
        const Program &program, const glm::mat3x3 matrix, const bool transpose
    ) const noexcept {
        glProgramUniformMatrix3fv(program.id, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    }

    void Uniform::set_mat4x4(
        const Program &program, const glm::mat4x4 matrix, const bool transpose
    ) const noexcept {
        glProgramUniformMatrix4fv(program.id, location, 1, transpose ? GL_TRUE : GL_FALSE, glm::value_ptr(matrix));
    }

    void Uniform::set_mat2x2_array(
        const Program &program, const std::span<glm::mat2x2> data, const bool transpose
    ) const noexcept {
        glProgramUniformMatrix2fv(
            program.id,
            location,
            static_cast<GLsizei>(data.size()),
            transpose ? GL_TRUE : GL_FALSE,
            glm::value_ptr(data.front())
        );
    }

    void Uniform::set_mat3x3_array(
        const Program &program, const std::span<glm::mat3x3> data, const bool transpose
    ) const noexcept {
        glProgramUniformMatrix3fv(
            program.id,
            location,
            static_cast<GLsizei>(data.size()),
            transpose ? GL_TRUE : GL_FALSE,
            glm::value_ptr(data.front())
        );
    }

    void Uniform::set_mat4x4_array(
        const Program &program, const std::span<glm::mat4x4> data, const bool transpose
    ) const noexcept {
        glProgramUniformMatrix4fv(
            program.id,
            location,
            static_cast<GLsizei>(data.size()),
            transpose ? GL_TRUE : GL_FALSE,
            glm::value_ptr(data.front())
        );
    }
}
