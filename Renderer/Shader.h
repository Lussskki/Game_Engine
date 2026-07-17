#pragma once

#include <array>
#include <string>

namespace Engine
{

class Shader
{
public:
    Shader() = default;
    ~Shader();

    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void Bind() const;
    void Unbind() const;
    void SetMat4(const std::string& name, const std::array<float, 16>& value) const;
    void SetInt(const std::string& name, int value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;

private:
    static std::string ReadFile(const std::string& path);
    static unsigned int Compile(unsigned int type, const std::string& source, const std::string& path);

    unsigned int m_ProgramId = 0;
};

}

