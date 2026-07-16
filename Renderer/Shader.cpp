#include "Renderer/Shader.h"

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

namespace Engine
{

Shader::~Shader()
{
    if (m_ProgramId != 0)
    {
        glDeleteProgram(m_ProgramId);
    }
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    const std::string vertexSource = ReadFile(vertexPath);
    const std::string fragmentSource = ReadFile(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty())
    {
        return false;
    }

    const unsigned int vertexShader = Compile(GL_VERTEX_SHADER, vertexSource, vertexPath);
    const unsigned int fragmentShader = Compile(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
    if (vertexShader == 0 || fragmentShader == 0)
    {
        return false;
    }

    m_ProgramId = glCreateProgram();
    glAttachShader(m_ProgramId, vertexShader);
    glAttachShader(m_ProgramId, fragmentShader);
    glLinkProgram(m_ProgramId);

    int success = 0;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &success);
    if (!success)
    {
        int length = 0;
        glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(static_cast<size_t>(length));
        glGetProgramInfoLog(m_ProgramId, length, nullptr, log.data());
        std::cerr << "Shader link error:\n" << log.data() << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void Shader::Bind() const
{
    glUseProgram(m_ProgramId);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetMat4(const std::string& name, const std::array<float, 16>& value) const
{
    const int location = glGetUniformLocation(m_ProgramId, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, value.data());
}

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cerr << "Failed to open shader: " << path << std::endl;
        return {};
    }

    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

unsigned int Shader::Compile(unsigned int type, const std::string& source, const std::string& path)
{
    const unsigned int shader = glCreateShader(type);
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(static_cast<size_t>(length));
        glGetShaderInfoLog(shader, length, nullptr, log.data());
        std::cerr << "Shader compile error in " << path << ":\n" << log.data() << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

}
