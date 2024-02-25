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

    BufferInfo(BufferType bufferType, int binding, int size) : m_BufferType(bufferType), m_Binding(binding), m_Size(size) {}
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

    inline void AddVariable(const std::string &name, int uniformOffset)
    {
        m_Variables[name] = uniformOffset;
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
