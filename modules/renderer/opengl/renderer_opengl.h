#include <rockit/platform/platform.h>
#include <rockit/graphics/renderer.h>

// #include <SDL2/SDL_opengl.h>
#ifdef __APPLE__
    #include <GL/glew.h>
#else
    #include <GL/glew.h>
    #include <GL/gl.h>
#endif

namespace Rockit
{
    class OpenGLTexture : public Texture
    {
    protected:
        GLuint textureHandle = 0;
    public:
        OpenGLTexture(uint32_t width, uint32_t height, Texture::Format format)
                : Texture(width, height, format)
        {
            GLint internalFormat = 0;
            GLint dataFormat = 0;
            GLint dataType = 0;

            auto newFormat = format;

            // for now, we treat the default as using back buffer formats
            if(newFormat == Texture::Format::Backbuffer || newFormat == Texture::Format::Default)
            {
                auto applicationInstance = Application::GetApplicationInstance();
                auto backBuffer = applicationInstance->GetBackBuffer();
                auto backBufferTex = backBuffer->GetTexture();
                auto backBufferFormat = backBufferTex->GetFormat();
                newFormat = backBufferFormat;
            }

            switch(newFormat)
            {
                case Texture::Format::R8U:
                    dataFormat = GL_R;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case Texture::Format::R32F:
                    dataFormat = GL_R;
                    dataType = GL_FLOAT;
                    break;
                case Texture::Format::RGB8U:
                    dataFormat = GL_RGB;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case Texture::Format::RGB8S:
                    dataFormat = GL_RGB;
                    dataType = 	GL_BYTE;
                    break;
                case Texture::Format::RGBA8U:
                    dataFormat = GL_RGBA;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case Texture::Format::RGBA8S:
                    dataFormat = GL_RGBA;
                    dataType = 	GL_BYTE;
                    break;
                case Texture::Format::RGBA32F:
                    dataFormat = GL_RGBA;
                    dataType = GL_FLOAT;
                    break;
                case Texture::Format::SRGB8:
                    dataFormat = GL_SRGB;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                case Texture::Format::SRGBA8:
                    dataFormat = GL_SRGB_ALPHA;
                    dataType = GL_UNSIGNED_BYTE;
                    break;
                default:
                    // invalid format
                    break;
            }

            if(newFormat != Texture::Format::None)
            {
                internalFormat = dataFormat;
                glGenTextures(1, &textureHandle);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureHandle);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (GLint)width, (GLint)height, 0, dataFormat, dataType, nullptr);
            }
        }
        ~OpenGLTexture() override
        {
            if (textureHandle != 0)
            {
                glDeleteTextures(1, &textureHandle);
            }
        }

    };

    class OpenGLShader : public Shader
    {
    protected:
        GLuint id;
    public:

        OpenGLShader() = delete;
        OpenGLShader(const OpenGLShader&) = delete;
        OpenGLShader& operator=(const OpenGLShader&) = delete;
        OpenGLShader& operator=(OpenGLShader&&) = delete;

        OpenGLShader(Shader::Description description)
        : Shader(description)
        {
            GLint shaderType;

            switch(type)
            {
                case Shader::Type::Vertex:
                    shaderType = GL_VERTEX_SHADER;
                    break;
                case Shader::Type::Fragment:
                    shaderType = GL_FRAGMENT_SHADER;
                    break;
                case Shader::Type::Compute:
                    shaderType = GL_COMPUTE_SHADER;
                    break;
                case Shader::Type::Geometry:
                    shaderType = GL_GEOMETRY_SHADER;
                    break;
            }
            id = glCreateShader( shaderType );
            glShaderSource(id, 1, &source, nullptr );
            glCompileShader( id );

        }
        virtual ~OpenGLShader() override
        {

        }
    };

    class OpenGLRenderer : public Renderer
    {
    public:
        OpenGLRenderer()
        {
            backendApi = Renderer::Backend::OpenGL;

            auto init_res = glewInit();
            if(init_res != GLEW_OK)
            {
//                std::cout << glewGetErrorString(glewInit()) << std::endl;
            }
        };
        virtual ~OpenGLRenderer() {};

        SharedPointer<Texture> CreateTexture(uint32_t width, uint32_t height, Texture::Format format)
        {
            auto texture = MakeSharedPointer<OpenGLTexture>(width, height, format);
            return texture;
        };
        SharedPointer<RenderTarget> CreateRenderTarget(uint32_t width, uint32_t height)
        {
            return SharedPointer<RenderTarget>(nullptr);
        };
        SharedPointer<Shader> CreateShader(Shader::Description description)
        {
            return MakeSharedPointer<OpenGLShader>(description);
        };

    };
}