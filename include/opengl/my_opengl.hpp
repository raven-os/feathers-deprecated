#ifndef MY_OPENGL_HPP_
# define MY_OPENGL_HPP_

# include <string>
# include <array>
# include <GLES3/gl3.h>

class Shader
{
public:
  GLuint shader;
private:
  unsigned int *count;
public:
  Shader(GLuint);
  ~Shader();
  Shader(Shader const &);
  Shader &operator=(Shader);
  operator GLuint() const;
};

class Program
{
public:
  GLuint program;
private:
  unsigned int *count;
public:
  Program();
  ~Program();
  Program(Program const &);
  Program &operator=(Program);
  operator GLuint() const;
};

class Vao
{
public:
  GLuint vao;
private:
  unsigned int *count;
public:
  Vao();
  ~Vao();
  Vao(Vao const &);
  Vao &operator=(Vao);
  operator GLuint() const;
};

class glBuffer
{
public:
  GLuint buffer;
private:
  unsigned int *count;
public:
  glBuffer();
  ~glBuffer();
  glBuffer(glBuffer const &);
  glBuffer &operator=(glBuffer);
  operator GLuint() const;
};

class Framebuffer
{
public:
  GLuint framebuffer;
private:
  unsigned int *count;
public:
  Framebuffer();
  ~Framebuffer();
  Framebuffer(Framebuffer const &);
  Framebuffer &operator=(Framebuffer);
  operator GLuint() const;
};

class Texture
{
public:
  GLuint texture;
private:
  unsigned int *count;
public:
  Texture();
  ~Texture();
  Texture(Texture const &);
  Texture &operator=(Texture);
  operator GLuint() const;
};

namespace	my_opengl
{
  void		shaderError(GLenum const shadertype, GLuint const shader);
  Shader	createShader(GLenum const shadertype, GLchar const *src);
  void		programError(GLuint const program);

  template<unsigned int count>
  Program createProgram(std::array<Shader const, count> const shaders)
  {
    Program program;
    GLint status;

    for (unsigned int i(0); i < count; ++i)
      glAttachShader(program, shaders[i]);
    glLinkProgram(program);
    for (unsigned int i(0); i < count; ++i)
      glDetachShader(program, shaders[i]);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
      programError(program);
    return (program);
  }

  Texture loadTexture(std::string const &name);
};

#endif // MY_OPENGL_HPP_
