#ifndef OPENGL_STUDY_BUFFER_INFO_H
#define OPENGL_STUDY_BUFFER_INFO_H

#include <unordered_map>
#include <string>

class BufferInfo
{
public:
    enum class BufferType
    {
        UNIFORM,
        SHADER_STORAGE
    };

    BufferInfo(BufferType bufferType, int binding, int size, std::unordered_map<std::string, int> variables) : m_BufferType(bufferType), m_Binding(binding), m_Size(size), m_Variables(std::move(variables)) {}
    ~BufferInfo() = default;

    inline BufferType GetBufferType() const
    {
        return m_BufferType;
    }

    inline int GetBinding() const
    {
        return m_Binding;
    }

    inline int GetSize() const
    {
        return m_Size;
    }

    inline const std::unordered_map<std::string, int> &GetVariables() const
    {
        return m_Variables;
    }

    BufferInfo(const BufferInfo &) = delete;
    BufferInfo(BufferInfo &&) = delete;

    BufferInfo &operator()(const BufferInfo &) = delete;
    BufferInfo &operator()(BufferInfo &&) = delete;

private:
    BufferType m_BufferType;
    int m_Binding;
    int m_Size;

    std::unordered_map<std::string, int> m_Variables;
};

#endif //OPENGL_STUDY_BUFFER_INFO_H
