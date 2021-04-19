#pragma once

#include "pch.hpp"

namespace lwvl {
    class Uniform {
        int m_location = -1;

    public:
        Uniform() = default;

        explicit Uniform(int location);

        Uniform(const Uniform &other) = default;

        Uniform(Uniform &&other) = default;

        ~Uniform() = default;

        Uniform &operator=(const Uniform &other) = default;

        Uniform &operator=(Uniform &&other) = default;

        void set1i(int i0);

        void set1f(float f0);

        void set1u(unsigned int u0);

        void set2i(int i0, int i1);

        void set2f(float f0, float f1);

        void set2u(unsigned int u0, unsigned int u1);

        void set3i(int i0, int i1, int i2);

        void set3f(float f0, float f1, float f2);

        void set3u(unsigned int u0, unsigned int u1, unsigned int u2);

        void set4i(int i0, int i1, int i2, int i3);

        void set4f(float f0, float f1, float f2, float f3);

        void set4u(unsigned int u0, unsigned int u1, unsigned int u2, unsigned int u3);

        void setMatrix4(const float *data);

        void setOrthographic(float top, float bottom, float right, float left, float far, float near);

        void set2DOrthographic(float top, float bottom, float right, float left);

        int location() const;
    };


    class shader_compilation_failure : public std::exception {
    public:
        explicit shader_compilation_failure(const std::string &msg);
    };


    enum class ShaderType {
        Vertex = GL_VERTEX_SHADER,
        TessCtrl = GL_TESS_CONTROL_SHADER,
        TessEval = GL_TESS_EVALUATION_SHADER,
        Geometry = GL_GEOMETRY_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute = GL_COMPUTE_SHADER
    };


    template<ShaderType target>
    class Shader {
        unsigned int m_id{reserve()};

        friend class ShaderProgram;

        static unsigned int reserve() {
            unsigned int id = glCreateShader(static_cast<GLenum>(target));
            return id;
        }

    public:
        explicit Shader(const std::string &source) {
            const char *src = source.c_str();
            glShaderSource(m_id, 1, &src, nullptr);
            glCompileShader(m_id);

            int result;
            glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);
            if (result == GL_FALSE) {
                int length;
                glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
                char *message = static_cast<char *>(_malloca(length * sizeof(char)));
                glGetShaderInfoLog(m_id, length, &length, message);
                std::stringstream error;
                error << "Failed to compile ";

                switch (target) {
                    case ShaderType::Vertex: error << "vertex";
                        break;
                    case ShaderType::Fragment: error << "fragment";
                        break;
                    default: error << "unknown type of ";
                        break;
                }

                error << " shader" << std::endl << message << std::endl;
                throw std::invalid_argument(error.str().c_str());
            }
        }

        Shader(const Shader &other) = delete;

        Shader(Shader &&other) noexcept: m_id(other.m_id) {
            other.m_id = 0;
        }

        Shader &operator=(const Shader &other) = delete;

        Shader &operator=(Shader &&other) noexcept {
            m_id = other.m_id;
            other.m_id = 0;
            return *this;
        }

        static std::string readFile(const std::string &filepath) {
            // need to figure out how to handle errors on this.
            std::ifstream file(filepath);
            std::stringstream output_stream;

            std::string line;
            while (getline(file, line)) {
                output_stream << line << '\n';
            }

            return output_stream.str();
        }

        ~Shader() {
            // An id of 0 will be silently ignored.
            glDeleteShader(m_id);
        }
    };


    /* ****** Shader Types ****** */
    typedef Shader<ShaderType::Vertex> VertexShader;
    typedef Shader<ShaderType::TessCtrl> TesselationControlShader;
    typedef Shader<ShaderType::TessEval> TesselationEvaluationShader;
    typedef Shader<ShaderType::Geometry> GeometryShader;
    typedef Shader<ShaderType::Fragment> FragmentShader;
    typedef Shader<ShaderType::Compute> ComputeShader;


    /* ****** Shader Program ******
    * A simple abstraction over the code required to create a shader in OpenGL.
    * Not feature complete.
    *
    * Usage:
    *   ShaderProgram myShader;
    *   myShader.attach(VertexShader);
    *   myShader.attach(GeometryShader);
    *   myShader.attach(FragmentShader);
    *   myShader.link();
    *
    * Quick Linking (Vertex and Fragment Shaders only):
    *   ShaderProgram myShader;
    *   myShader.link(VertexShader, FragmentShader);
    */
    class ShaderProgram {
        class ID {
            static unsigned int reserve() {
                return glCreateProgram();
            }

        public:
            ~ID() {
                glDeleteProgram(programID);
            }

            constexpr explicit operator uint32_t() const {
                return programID;
            }

            const uint32_t programID = reserve();
        };

        // Offsite Data -
        std::shared_ptr<ShaderProgram::ID> m_offsite_id = std::make_shared<ShaderProgram::ID>();

        // Local Data
        uint32_t m_id = static_cast<uint32_t>(*m_offsite_id);

        [[nodiscard]] int uniformLocation(const std::string &name) const;

    public:
        ShaderProgram() = default;

        ShaderProgram(const ShaderProgram &other) = default;

        ShaderProgram &operator=(const ShaderProgram &other) = default;

        ShaderProgram(ShaderProgram &&other) noexcept = default;

        ShaderProgram &operator=(ShaderProgram &&other) noexcept = default;

        [[nodiscard]] unsigned int id() const;

        Uniform uniform(const std::string &name);

        template<ShaderType target>
        void attach(const Shader<target> &shader) { glAttachShader(m_id, shader.m_id); }

        template<ShaderType target>
        void detach(const Shader<target> &shader) { glDetachShader(m_id, shader.m_id); }

        void link();

        void link(const VertexShader &vs, const FragmentShader &fs);

        void link(const std::string &vertexSource, const std::string &fragmentSource);

        void bind() const;

        static void clear();
    };
}
