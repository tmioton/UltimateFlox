#pragma once

// Figure out how to make this library independent.
#include <glad/glad.h>

#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <variant>

namespace lwvl {
    class Buffer;

    class Framebuffer;

    class Texture;

    class Program;

    class Uniform;

    class VertexArray;

    enum class Attachment {
        Color = GL_COLOR_ATTACHMENT0,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT
    };

    enum class ByteFormat {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        HalfFloat = GL_HALF_FLOAT,
        Float = GL_FLOAT,
        Fixed = GL_FIXED,
        Double = GL_DOUBLE,
        Int_2_10_10_10 = GL_INT_2_10_10_10_REV,
        UnsignedInt_2_10_10_10 = GL_UNSIGNED_INT_2_10_10_10_REV,
        UnsignedInt_10F_11F_11F = GL_UNSIGNED_INT_10F_11F_11F_REV
    };

    enum class ChannelLayout {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,

        R8 = GL_R8,
        R16 = GL_R16,
        RG8 = GL_RG8,
        RG16 = GL_RG16,
        RGB8 = GL_RGB8,
        RGBA2 = GL_RGBA2,
        RGBA4 = GL_RGBA4,
        RGBA8 = GL_RGBA8,
        RGBA16 = GL_RGBA16,

        // Half Float Formats
        R16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,

        // Full Float Formats
        R32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,

        // Signed Integer Formats
        R8I = GL_R8I,
        RG8I = GL_RG8I,
        RGB8I = GL_RGB8I,
        RGBA8I = GL_RGBA8I,

        R16I = GL_R16I,
        RG16I = GL_RG16I,
        RGB16I = GL_RGB16I,
        RGBA16I = GL_RGBA16I,

        R32I = GL_R32I,
        RG32I = GL_RG32I,
        RGB32I = GL_RGB32I,
        RGBA32I = GL_RGBA32I,

        // Unsigned Integer Formats
        R8UI = GL_R8UI,
        RG8UI = GL_RG8UI,
        RGB8UI = GL_RGB8UI,
        RGBA8UI = GL_RGBA8UI,

        R16UI = GL_R16UI,
        RG16UI = GL_RG16UI,
        RGB16UI = GL_RGB16UI,
        RGBA16UI = GL_RGBA16UI,

        R32UI = GL_R32UI,
        RG32UI = GL_RG32UI,
        RGB32UI = GL_RGB32UI,
        RGBA32UI = GL_RGBA32UI,
    };

    enum class ChannelOrder {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,
        StencilIndex = GL_STENCIL_INDEX,
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL
    };

    enum class Filter {
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST
    };

    enum class PrimitiveMode {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,
        LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
        Triangles = GL_TRIANGLES,
        TriangleFan = GL_TRIANGLE_FAN,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
        TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
    };

    namespace bits {
        constexpr GLbitfield None = 0;
        typedef enum GLbitfield {
            Dynamic = GL_DYNAMIC_STORAGE_BIT,
            MapRead = GL_MAP_READ_BIT,
            MapWrite = GL_MAP_WRITE_BIT,
            MapPersistent = GL_MAP_PERSISTENT_BIT,
            MapCoherent = GL_MAP_COHERENT_BIT,
            Client = GL_MAP_COHERENT_BIT
        } UsageBits;
    }

    class Buffer {
    public:
        enum class Target {
            Array = GL_ARRAY_BUFFER,
            Element = GL_ELEMENT_ARRAY_BUFFER,
            Texture = GL_TEXTURE_BUFFER,
            //Uniform = GL_UNIFORM_BUFFER,
            Shader = GL_SHADER_STORAGE_BUFFER,
        };

        enum class IndexedTarget {
            AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
            ShaderStorage = GL_SHADER_STORAGE_BUFFER,
            TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
            Uniform = GL_UNIFORM_BUFFER,
        };

        enum class Usage {
            Static = GL_STATIC_DRAW,
            Dynamic = GL_DYNAMIC_DRAW,
            Stream = GL_STREAM_DRAW
        };
    private:
        class ID {
            static GLuint reserve();

        public:
            ~ID();

            const GLuint id = reserve();
        };

        // Offsite Data - Maintain lifetime of the buffer over copies.
        std::shared_ptr<const ID> m_offsite_id = std::make_shared<const ID>();

    public:
        [[nodiscard]] GLuint id() const;

        void bind(Target);

        void bind(IndexedTarget, GLuint);

        static void clear(Target);

        Buffer() = default;

        Buffer(Buffer const &) = default;

        Buffer(Buffer &&) noexcept = default;

        Buffer &operator=(Buffer const &) = default;

        Buffer &operator=(Buffer &&) noexcept = default;

        template<typename T>
        void construct(T const *data, GLsizeiptr size, Usage usage = Usage::Dynamic) {
            glNamedBufferData(id(), size, data, static_cast<GLenum>(usage));
        }

        template<class Iterator>
        void construct(Iterator first, Iterator last, Usage usage = Usage::Dynamic) {
            glNamedBufferData(id(), sizeof(*first) * (last - first), &(*first), static_cast<GLenum>(usage));
        }

        template<typename T>
        void store(T const *data, GLsizeiptr size, GLbitfield usage = bits::None) {
            glNamedBufferStorage(id(), size, data, usage);
        }

        template<class Iterator>
        void store(Iterator first, Iterator last, GLbitfield usage = bits::None) {
            glNamedBufferStorage(id(), sizeof(*first) * (last - first), &(*first), usage);
        }

        template<typename T>
        void update(T const *data, GLsizeiptr size, GLsizei offset = 0) {
            glNamedBufferSubData(id(), offset, size, data);
        }

        template<class Iterator>
        void update(Iterator first, Iterator last, GLsizei offset = 0) {
            glNamedBufferSubData(id(), offset, sizeof(*first) * (last - first), &(*first));
        }
    };

    namespace debug {
        enum class Source {
            API = GL_DEBUG_SOURCE_API,
            APPLICATION = GL_DEBUG_SOURCE_APPLICATION,
            SHADER_COMPILER = GL_DEBUG_SOURCE_SHADER_COMPILER,
            THIRD_PARTY = GL_DEBUG_SOURCE_THIRD_PARTY,
            WINDOW_SYSTEM = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
            OTHER = GL_DEBUG_SOURCE_OTHER,
            DONT_CARE = GL_DONT_CARE
        };

        enum class Type {
            DEPRECATED_BEHAVIOR = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
            ERROR = GL_DEBUG_TYPE_ERROR,
            MARKER = GL_DEBUG_TYPE_MARKER,
            PERFORMANCE = GL_DEBUG_TYPE_PERFORMANCE,
            PORTABILITY = GL_DEBUG_TYPE_PORTABILITY,
            POP_GROUP = GL_DEBUG_TYPE_POP_GROUP,
            PUSH_GROUP = GL_DEBUG_TYPE_PUSH_GROUP,
            UNDEFINED_BEHAVIOR = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
            OTHER = GL_DEBUG_TYPE_OTHER,
            DONT_CARE = GL_DONT_CARE
        };

        enum class Severity {
            LOW = GL_DEBUG_SEVERITY_LOW,
            MEDIUM = GL_DEBUG_SEVERITY_MEDIUM,
            HIGH = GL_DEBUG_SEVERITY_HIGH,
            NOTIFICATION = GL_DEBUG_SEVERITY_NOTIFICATION,
            DONT_CARE = GL_DONT_CARE
        };

#ifdef _WIN32
        using LWVLDebugProc = void (__stdcall *)(
#else
            using LWVLDebugProc = void(*)(
#endif
            Source source, Type type, Severity severity,
            unsigned int id, int length, const char *message,
            const void *userState
        );

        class GLEventListener {
            GLuint m_unusedIDs = 0;
            LWVLDebugProc m_callback;
            void *m_userPtr;

            void assign();

        public:
            explicit GLEventListener(void *userPtr, LWVLDebugProc callback, bool enabled = true);

            explicit GLEventListener(LWVLDebugProc callback, bool enabled = true);

            ~GLEventListener();

            void control(Source sourceFilter, Type typeFilter, Severity severityFilter, GLsizei count, bool enabled);

            void control(bool enabled);

            void invoke(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message);
        };

        static void __stdcall glDebugCallback(
            GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
            const void *state
        );

        void simpleDebugCallback(GLDEBUGPROC callback, void *userPtr);

        void clearErrors();
    }

    void clear();

    class Framebuffer {
        class ID {
            GLuint reserve();

        public:
            ID() = default;
            ID(int);
            ~ID();

            GLuint id = reserve();
            bool safe = true;
        };

        std::shared_ptr<const ID> m_offsite_id = std::make_shared<const ID>();

        explicit Framebuffer(int);
    public:
        [[nodiscard]] GLuint id() const;

        void bind();

        static void clear();

        // DSA textures know what type they are, so we don't need attach1D, attach2D, etc.
        void attach(Attachment point, Texture const &texture, GLint level);

        void attachLayer(Attachment point, Texture const &texture, GLint level, GLint layer);

        static Framebuffer activeDrawFramebuffer();
        static Framebuffer activeReadFramebuffer();
        bool safe();
    };

    struct Region {
        GLsizei length;
        GLint offset;

        explicit Region(GLsizei l, GLint o = 0);
    };

    class Texture {
        friend Framebuffer;
    public:
        enum class Target {
            Texture1D = GL_TEXTURE_1D,
            Texture1DArray = GL_TEXTURE_1D_ARRAY,
            Texture2D = GL_TEXTURE_2D,
            Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
            Texture2DArray = GL_TEXTURE_2D_ARRAY,
            Texture2DArrayMultisample = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TextureRectangle = GL_TEXTURE_RECTANGLE,
            TextureCubeMap = GL_TEXTURE_CUBE_MAP,
            TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
            Texture3D = GL_TEXTURE_3D,
            TextureBuffer = GL_TEXTURE_BUFFER
        };

        class ID {
        protected:
            static GLuint reserve(GLenum target);

        public:
            explicit ID(Target);

            ~ID();

            const GLuint id;
        };

    private:
        std::shared_ptr<const ID> m_offsite_id;
        Target m_target;

    public:
        [[nodiscard]] GLuint id() const;

        [[nodiscard]] Target target() const;

        Texture();  // Default to 2D texture
        explicit Texture(Target t);

        // Add view thingy

        void bind(GLuint slot);

        static void clear(GLuint slot);

        void filter(Filter value);

        // Set the format of the texture.
        void format(GLsizei width, lwvl::ChannelLayout layout, GLsizei levels = 1);

        void format(GLsizei width, GLsizei height, lwvl::ChannelLayout layout, GLsizei levels = 1);

        void format(GLsizei width, GLsizei height, GLsizei depth, lwvl::ChannelLayout layout, GLsizei levels = 1);

        void format(Buffer const &, ChannelLayout);

        void format(Buffer const &, ChannelLayout, GLsizeiptr, GLintptr = 0);

        void construct(
            Region x, GLsizei level,
            lwvl::ChannelOrder order, lwvl::ByteFormat type,
            void const *pixels
        );

        void construct(
            Region x, Region y, GLsizei level,
            lwvl::ChannelOrder order, lwvl::ByteFormat type,
            void const *pixels
        );

        void construct(
            Region x, Region y, Region z, GLsizei level,
            lwvl::ChannelOrder order, lwvl::ByteFormat type,
            void const *pixels
        );

        void construct(
            GLsizei width, GLsizei level,
            lwvl::ChannelOrder order, lwvl::ByteFormat type,
            void const *pixels
        );

        void construct(
            GLsizei width, GLsizei height, GLsizei level,
            lwvl::ChannelOrder order, lwvl::ByteFormat type,
            void const *pixels
        );

        void construct(
            GLsizei width, GLsizei height, GLsizei depth, GLsizei level,
            lwvl::ChannelOrder order, lwvl::ByteFormat type,
            void const *pixels
        );
    };

    namespace details {
        enum class ShaderType {
            Vertex = GL_VERTEX_SHADER,
            Fragment = GL_FRAGMENT_SHADER,
            Geometry = GL_GEOMETRY_SHADER,
            TessellationControl = GL_TESS_CONTROL_SHADER,
            TessellationEvaluation = GL_TESS_EVALUATION_SHADER,
            Compute = GL_COMPUTE_SHADER
        };

        static std::ostream &operator<<(std::ostream &os, ShaderType target) {
            switch (target) {
                case details::ShaderType::Vertex: os << "vertex";
                    break;
                case details::ShaderType::Fragment: os << "fragment";
                    break;
                case details::ShaderType::Geometry: os << "geometry";
                    break;
                case details::ShaderType::TessellationControl: os << "tessellation control";
                    break;
                case details::ShaderType::TessellationEvaluation: os << "tessellation evaluation";
                    break;
                case details::ShaderType::Compute:
                default: os << "compute";
                    break;
            }
            return os;
        }

        template<ShaderType target>
        class Shader {
            // Offsite is a little overkill here
            class ID {
                static GLuint reserve() {
                    return glCreateShader(static_cast<GLenum>(target));
                }

            public:
                ~ID() { glDeleteShader(id); }

                const GLuint id = reserve();
            };

            std::shared_ptr<const ID> m_offsite_id = std::make_shared<const ID>();
        public:
            [[nodiscard]] GLuint id() const {
                return m_offsite_id->id;
            }

            explicit Shader(std::string const &source) {
                const char *src = source.c_str();
                const GLuint so = id();
                int result;
                glShaderSource(so, 1, &src, nullptr);
                glCompileShader(so);

                glGetShaderiv(so, GL_COMPILE_STATUS, &result);
                if (result == GL_FALSE) {
                    int length;
                    glGetShaderiv(so, GL_INFO_LOG_LENGTH, &length);
                    char *infoLog = new char[length];
                    glGetShaderInfoLog(so, length, &length, infoLog);
                    std::stringstream error;
                    error << "Failed to compile " << target << " shader:" << std::endl << infoLog << std::endl;
                    throw std::invalid_argument(error.str().c_str());
                }
            }

            Shader(Shader const &) = default;

            Shader(Shader &&) noexcept = default;

            Shader &operator=(Shader const &) = default;

            Shader &operator=(Shader &&) noexcept = default;

            ~Shader() = default;

            static std::string readFile(const std::string &path) {
                // Need to figure out how to handle errors on this.
                // Compilation acts as a sort of error handling.
                std::ifstream file(path);
                std::stringstream output_stream;

                std::string line;
                while (getline(file, line)) {
                    output_stream << line << '\n';
                }

                // No need to return std::string&& since we need to copy
                // from output_stream anyway.
                return output_stream.str();
            }

            static Shader<target> fromFile(const std::string &path) {
                return Shader<target>(readFile(path));
            }
        };
    }

    using VertexShader = details::Shader<details::ShaderType::Vertex>;
    using FragmentShader = details::Shader<details::ShaderType::Fragment>;
    using GeometryShader = details::Shader<details::ShaderType::Geometry>;
    using TessControlShader = details::Shader<details::ShaderType::TessellationControl>;
    using TessEvaluationShader = details::Shader<details::ShaderType::TessellationEvaluation>;
    using ComputeShader = details::Shader<details::ShaderType::Compute>;

    class Program {
        class ID {
            static GLuint reserve();

        public:
            ~ID();

            const GLuint id = reserve();
        };

        [[nodiscard]] int uniformLocation(const char *name) const;

        std::shared_ptr<const ID> m_offsite_id = std::make_shared<const ID>();
    public:
        [[nodiscard]] GLuint id() const;

        Program() = default;

        Program(Program const &) = default;

        Program(Program &&) noexcept = default;

        Program &operator=(Program const &) = default;

        Program &operator=(Program &&) = default;

        Uniform uniform(const char *name);

        template<details::ShaderType target>
        void attach(details::Shader<target> const &shader) {
            glAttachShader(id(), shader.id());
        }

        template<details::ShaderType target>
        void detach(details::Shader<target> const &shader) {
            glAttachShader(id(), shader.id());
        }

        void link();

        void link(VertexShader const &vs, FragmentShader const &fs);

        void link(std::string const &vertexSource, std::string const &fragmentSource);

        void bind() const;

        static void clear();
        static int active();
    };

    class Uniform {
        GLint m_location = -1;
    public:
        Uniform() = default;

        explicit Uniform(GLint location);

        Uniform(Uniform const &) = default;

        Uniform(Uniform &&) = default;

        Uniform &operator=(Uniform const &) = default;

        Uniform &operator=(Uniform &&) = default;

        // Separate setters since set(1) is ambiguous.
        void setI(GLint);

        void setI(GLint, GLint);

        void setI(GLint, GLint, GLint);

        void setI(GLint, GLint, GLint, GLint);

        void setU(GLuint);

        void setU(GLuint, GLuint);

        void setU(GLuint, GLuint, GLuint);

        void setU(GLuint, GLuint, GLuint, GLuint);

        void setF(float);

        void setF(float, float);

        void setF(float, float, float);

        void setF(float, float, float, float);

        void matrix4F(const float *data);

        void ortho(float top, float bottom, float right, float left, float far, float near);

        void ortho2D(float top, float bottom, float right, float left);

        void ortho2D(float aspect);

        [[nodiscard]] GLint location() const;
    };

    /*
     *
     * Combined index/vertex buffer:
     * https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions#storing-index-and-vertex-data-under-single-buffer
     */
    class VertexArray {
        class ID {
            static GLuint reserve();

        public:
            ~ID();

            const GLuint id = reserve();
        };

        void _format(GLuint vao, GLuint index, uint8_t dimensions, ByteFormat type, GLuint offset);

        std::shared_ptr<const ID> m_offsite_id = std::make_shared<const ID>();
    public:
        unsigned int instances = 1;

        [[nodiscard]] GLuint id() const;

        void bind();

        static void clear();

        VertexArray() = default;

        VertexArray(VertexArray const &) = default;

        VertexArray(VertexArray &&) noexcept = default;

        VertexArray &operator=(VertexArray const &) = default;

        VertexArray &operator=(VertexArray &&) noexcept = default;

        ~VertexArray() = default;

        // Set the element buffer for the vertex array.
        void element(lwvl::Buffer const &);

        // Attach an array buffer to a bind point.
        void array(lwvl::Buffer const &, GLuint binding, GLintptr offset, GLsizei stride);

        // Set the format for a section of a buffer.
        void attribute(GLuint index, uint8_t dimensions, ByteFormat type, GLuint offset);

        void attribute(GLuint binding, GLuint attribute, uint8_t dimensions, ByteFormat type, GLuint offset);

        // Mark that the specified format applies to the buffer bound to the specified bind point.
        void attach(GLuint binding, GLuint attribute);

        void divisor(GLuint binding, GLuint divisor);

        void enable(GLuint attribute);

        void disable(GLuint attribute);

        void drawArrays(PrimitiveMode mode, int count) const;

        void drawElements(PrimitiveMode mode, int count, ByteFormat type) const;
    };

    struct Viewport {
        int x, y, width, height;
    };

    void viewport(Viewport);

    Viewport viewport();
}
