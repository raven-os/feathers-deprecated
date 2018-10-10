#include <stdexcept>
#include <fstream>
#include <memory>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cassert>
#include "opengl/my_opengl.hpp"

namespace opengl
{
  void checkError()
  {
    if (int err = glGetError())
    {
      switch (err)
	{
	case GL_INVALID_ENUM:
	  std::cerr << "GL_INVALID_ENUM\n";
	  break;
	case GL_INVALID_VALUE:
	  std::cerr << "GL_INVALID_VALUE\n";
	  break;
	case GL_INVALID_OPERATION:
	  std::cerr << "GL_INVALID_OPERATION\n";
	  break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
	  std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
	  break;
	case GL_OUT_OF_MEMORY:
	  std::cerr << "GL_OUT_OF_MEMORY\n";
	  break;
	default:
	  std::cerr << "Unknown GL error\n";
	}
      assert(0);
    }
  }

  void shaderError(GLenum const shadertype, GLuint const shader)
  {
    GLint	len;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    std::unique_ptr<GLchar[]> log(new GLchar[len + 1]);

    log[len + 1] = 0; // safety
    glGetShaderInfoLog(shader, len, nullptr, &log[0]);
    throw std::runtime_error(std::string("Compilation failed for ")
			     + ((shadertype == GL_VERTEX_SHADER) ?
				"vertex" : (shadertype == GL_FRAGMENT_SHADER) ?
				"fragment" : "unknown (fix this in my_opengl.cpp!)")
			     + std::string(" shader: ")
			     + &log[0]);
  }

  void programError(GLuint const program)
  {
    GLint len;
    std::string log;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    log.reserve(static_cast<unsigned int>(len));
    glGetProgramInfoLog(program, len, NULL, &log[0]);
    throw std::runtime_error("link failure: " + log);
  }

  Shader createShader(GLenum const shadertype, GLchar const *src)
  {
    Shader shader(shadertype);
    GLint status(0);

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
      shaderError(shadertype, shader);
    return (shader);
  }

  Program createProgram(std::string const& name)
  {
    opengl::checkError(); // avoid propagating an error into the shader compilation
    std::stringstream vert;
    std::stringstream frag;
    std::ifstream vertInput("shaders/" + name + ".vert");
    std::ifstream fragInput("shaders/" + name + ".frag");

    if (!fragInput || !vertInput)
      {
	std::cout << "shaders/" + name + ".vert" << std::endl;
	std::cout << "shaders/" + name + ".frag" << std::endl;
	throw std::runtime_error(strerror(errno));
      }

    vert << vertInput.rdbuf();
    frag << fragInput.rdbuf();

    Shader vertex = createShader(GL_VERTEX_SHADER, vert.str().c_str());
    Shader fragment = createShader(GL_FRAGMENT_SHADER, frag.str().c_str());

    return createProgram<2>({vertex, fragment});
  }

  Shader::Shader(GLuint shadertype)
    : shader(glCreateShader(shadertype)), count(new unsigned int(1u))
  {
  }

  Shader::~Shader()
  {
    if (!--*count)
      {
	glDeleteShader(shader);
	delete count;
      }
  }

  Shader::Shader(Shader const &s)
    : shader(s.shader), count(s.count)
  {
    ++*count;
  }

  Shader &Shader::operator=(Shader s)
  {
    std::swap(s.count, count);
    std::swap(s.shader, shader);
    return *this;
  }

  Shader::operator GLuint() const
  {
    return shader;
  }

  Program::Program()
    : program(glCreateProgram()), count(new unsigned int(1u))
  {
  }

  Program::~Program()
  {
    if (!--*count)
      {
	glDeleteProgram(program);
	delete count;
      }
  }

  Program::Program(Program const &s)
    : program(s.program), count(s.count)
  {
    ++*count;
  }

  Program &Program::operator=(Program s)
  {
    std::swap(s.count, count);
    std::swap(s.program, program);
    return *this;
  }

  Program::operator GLuint() const
  {
    return program;
  }

  glBuffer::glBuffer()
    : buffer(0u), count(new unsigned int(1u))
  {
    glGenBuffers(1, &buffer);
  }

  glBuffer::~glBuffer()
  {
    if (!--*count)
      {
	glDeleteBuffers(1, &buffer);
	delete count;
      }
  }

  glBuffer::glBuffer(glBuffer const &s)
    : buffer(s.buffer), count(s.count)
  {
    ++*count;
  }

  glBuffer &glBuffer::operator=(glBuffer s)
  {
    std::swap(s.count, count);
    std::swap(s.buffer, buffer);
    return *this;
  }

  glBuffer::operator GLuint() const
  {
    return buffer;
  }

  Framebuffer::Framebuffer()
    : framebuffer(0u), count(new unsigned int(1u))
  {
    glGenFramebuffers(1, &framebuffer);
  }

  Framebuffer::~Framebuffer()
  {
    if (!--*count)
      {
	glDeleteFramebuffers(1, &framebuffer);
	delete count;
      }
  }

  Framebuffer::Framebuffer(Framebuffer const &s)
    : framebuffer(s.framebuffer), count(s.count)
  {
    ++*count;
  }

  Framebuffer &Framebuffer::operator=(Framebuffer s)
  {
    std::swap(s.count, count);
    std::swap(s.framebuffer, framebuffer);
    return *this;
  }

  Framebuffer::operator GLuint() const
  {
    return framebuffer;
  }

  Texture::Texture()
    : texture(0u), count(new unsigned int(1u))
  {
    glGenTextures(1, &texture);
  }

  Texture::~Texture()
  {
    if (!--*count)
      {
	glDeleteTextures(1, &texture);
	delete count;
      }
  }

  Texture::Texture(Texture const &s)
    : texture(s.texture), count(s.count)
  {
    ++*count;
  }

  Texture &Texture::operator=(Texture s)
  {
    std::swap(s.count, count);
    std::swap(s.texture, texture);
    return *this;
  }

  Texture::operator GLuint() const
  {
    return texture;
  }
}
