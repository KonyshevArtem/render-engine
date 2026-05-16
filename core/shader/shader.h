#ifndef RENDER_ENGINE_SHADER_H
#define RENDER_ENGINE_SHADER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <mutex>

#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "enums/primitive_type.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"
#include "types/graphics_backend_program_descriptor.h"
#include "resources/resource.h"

struct GraphicsBackendTextureInfo;
struct GraphicsBackendSamplerInfo;
struct GraphicsBackendTLASInfo;
class GraphicsBackendBufferInfo;
class DrawableGeometry;

class Shader : public Resource
{
public:
    Shader(std::filesystem::path path, std::vector<std::string> defines);
    virtual ~Shader();

    static void AddGlobalDefine(const std::string& define);
    static void RemoveGlobalDefine(const std::string& define);

    const GraphicsBackendProgram& GetProgram();
    const GraphicsBackendProgram& GetProgram(const std::shared_ptr<DrawableGeometry>& geometry);
    const GraphicsBackendProgram& GetProgram(const VertexAttributes& vertexAttributes, PrimitiveType primitiveType);

    inline const std::unordered_map<std::string, GraphicsBackendTextureInfo> &GetTextures() const
    {
        return m_Textures;
    }

    inline const std::unordered_map<std::string, GraphicsBackendSamplerInfo> &GetSamplers() const
    {
        return m_Samplers;
    }

    inline const std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &GetBuffers() const
    {
        return m_Buffers;
    }

    inline bool SupportInstancing() const
    {
        return m_SupportInstancing;
    }

    inline bool IsValid() const
    {
        return !m_Shaders.empty();
    }

private:
    std::filesystem::path m_Path;
    std::vector<std::string> m_Defines;

    std::vector<GraphicsBackendShaderObject> m_Shaders;
    std::unordered_map<size_t, GraphicsBackendProgram> m_Programs;

    ProgramType m_Type;
    std::string m_Name;
    bool m_SupportInstancing = false;
    ThreadGroupSize m_ThreadGroupSize;
    size_t m_LastGlobalDefinesHash;

    std::unordered_map<std::string, GraphicsBackendTextureInfo> m_Textures;
    std::unordered_map<std::string, GraphicsBackendSamplerInfo> m_Samplers;
    std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> m_Buffers;
    std::unordered_map<std::string, GraphicsBackendTLASInfo> m_TLASes;

    static size_t s_GlobalDefinesHash;
    static std::mutex s_GlobalDefinesMutex;
    static std::unordered_set<std::string> s_GlobalDefines;

    void Init();
    void DeInit();
    void CheckGlobalDefines();

    const GraphicsBackendProgram& GetOrCreateRenderProgram(const VertexAttributes& vertexAttributes, PrimitiveType primitiveType);
    const GraphicsBackendProgram& GetOrCreateComputeProgram();
};

#endif //RENDER_ENGINE_SHADER_H