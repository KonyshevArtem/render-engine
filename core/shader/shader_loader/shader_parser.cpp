#include "shader_parser.h"

#include <boost/json.hpp>

const std::string SHADER_NAMES[ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT]
        {
                "vertex",
                "geometry",
                "fragment"
        };

const std::unordered_map<std::string, BlendFactor> BLEND_FACTOR_MAP
        {
                {"Zero",               BlendFactor::ZERO},
                {"One",                BlendFactor::ONE},
                {"SrcColor",           BlendFactor::SRC_COLOR},
                {"OneMinusSrcColor",   BlendFactor::ONE_MINUS_SRC_COLOR},
                {"DstColor",           BlendFactor::DST_COLOR},
                {"OneMinusDstColor",   BlendFactor::ONE_MINUS_DST_COLOR},
                {"SrcAlpha",           BlendFactor::SRC_ALPHA},
                {"OneMinusSrcAlpha",   BlendFactor::ONE_MINUS_SRC_ALPHA},
                {"DstAlpha",           BlendFactor::DST_ALPHA},
                {"OneMinusDstAlpha",   BlendFactor::ONE_MINUS_DST_ALPHA},
                {"ConstColor",         BlendFactor::CONSTANT_COLOR},
                {"OneMinusConstColor", BlendFactor::ONE_MINUS_CONSTANT_COLOR},
                {"ConstAlpha",         BlendFactor::CONSTANT_ALPHA},
                {"OneMinusConstAlpha", BlendFactor::ONE_MINUS_CONSTANT_ALPHA},
                {"SrcAlphaSaturate",   BlendFactor::SRC_ALPHA_SATURATE},
                {"Src1Color",          BlendFactor::SRC1_COLOR},
                {"OneMinusSrc1Color",  BlendFactor::ONE_MINUS_SRC1_COLOR},
                {"Src1Alpha",          BlendFactor::SRC1_ALPHA},
                {"OneMinusSrc1Alpha",  BlendFactor::ONE_MINUS_SRC1_ALPHA},
        };

const std::unordered_map<std::string, DepthFunction> DEPTH_FUNCTION_MAP
        {
                {"Never",    DepthFunction::NEVER},
                {"Less",     DepthFunction::LESS},
                {"Equal",    DepthFunction::EQUAL},
                {"Lequal",   DepthFunction::LEQUAL},
                {"Greater",  DepthFunction::GREATER},
                {"NotEqual", DepthFunction::NOTEQUAL},
                {"Gequal",   DepthFunction::GEQUAL},
                {"Always",   DepthFunction::ALWAYS},
        };

const std::unordered_map<std::string, CullFace> CULL_FACE_MAP
        {
                {"None",  CullFace::NONE},
                {"Front", CullFace::FRONT},
                {"Back",  CullFace::BACK},
        };

const std::unordered_map<std::string, CullFaceOrientation> CULL_FACE_ORIENTATION_MAP
        {
                {"CW",  CullFaceOrientation::CLOCKWISE},
                {"CCW", CullFaceOrientation::COUNTER_CLOCKWISE},
        };


template<typename TEnum>
TEnum StringToEnum(std::string name, std::unordered_map<std::string, TEnum> map)
{
    auto it = map.find(name);
    if (it != map.end())
    {
        return it->second;
    }

    throw std::runtime_error("[ShaderLoader] Unsupported string value: " + name);
}

template<typename T>
bool TryGetValue(const boost::json::object &object, const std::string &key, T &value)
{
    auto it = object.find(key);
    bool found = it != object.end();
    if (found)
    {
        value = boost::json::value_to<T>(it->value());
    }

    return found;
}

template<typename T>
bool TryGetValue(const boost::json::object &object, const std::string &key, T &value, T defaultValue)
{
    bool found = TryGetValue(object, key, value);
    if (!found)
    {
        value = defaultValue;
    }

    return found;
}

GraphicsBackendVertexAttributeDescriptor tag_invoke(const boost::json::value_to_tag<GraphicsBackendVertexAttributeDescriptor> &,
                                                    boost::json::value const &vertexAttributeValue)
{
    auto &vertexAttributeObject = vertexAttributeValue.as_object();

    GraphicsBackendVertexAttributeDescriptor vertexAttribute{};

    TryGetValue(vertexAttributeObject, "index", vertexAttribute.Index);
    TryGetValue(vertexAttributeObject, "dimensions", vertexAttribute.Dimensions);
    TryGetValue(vertexAttributeObject, "dataType", reinterpret_cast<int&>(vertexAttribute.DataType));
    TryGetValue(vertexAttributeObject, "isNormalized", reinterpret_cast<bool&>(vertexAttribute.IsNormalized));
    TryGetValue(vertexAttributeObject, "stride", vertexAttribute.Stride);
    TryGetValue(vertexAttributeObject, "offset", vertexAttribute.Offset);

    return vertexAttribute;
}

namespace ShaderParser
{

    BlendInfo ParseBlendInfo(const boost::json::object &passInfoObject)
    {
        BlendInfo info{};

        boost::json::object blendInfoObject;
        if (TryGetValue(passInfoObject, "blend", blendInfoObject))
        {
            info.Enabled = true;

            std::string factor;
            if (TryGetValue(blendInfoObject, "SrcFactor", factor))
            {
                info.SourceFactor = StringToEnum(factor, BLEND_FACTOR_MAP);
            }
            if (TryGetValue(blendInfoObject, "DstFactor", factor))
            {
                info.DestinationFactor = StringToEnum(factor, BLEND_FACTOR_MAP);
            }
        }

        return info;
    }

    CullInfo ParseCullInfo(const boost::json::object &passInfoObject)
    {
        CullInfo info{CullFace::BACK, CullFaceOrientation::CLOCKWISE};

        std::string cullValue;
        if (TryGetValue(passInfoObject, "cull", cullValue))
        {
            info.Face = StringToEnum(cullValue, CULL_FACE_MAP);
        }

        std::string cullOrientationValue;
        if (TryGetValue(passInfoObject, "cullOrientation", cullOrientationValue))
        {
            info.Orientation = StringToEnum(cullOrientationValue, CULL_FACE_ORIENTATION_MAP);
        }

        return info;
    }

    DepthInfo ParseDepthInfo(const boost::json::object &passInfoObject)
    {
        bool writeDepth;
        DepthInfo info{!TryGetValue(passInfoObject, "zWrite", writeDepth) || writeDepth};

        std::string depthFunctionValue;
        if (TryGetValue(passInfoObject, "zTest", depthFunctionValue))
        {
            info.DepthFunction = StringToEnum(depthFunctionValue, DEPTH_FUNCTION_MAP);
        }

        return info;
    }

    PassInfo ParsePassInfo(const boost::json::object &passInfoObject)
    {
        PassInfo info;
        TryGetValue(passInfoObject, "tags", info.Tags);

        info.DepthInfo = ParseDepthInfo(passInfoObject);
        info.BlendInfo = ParseBlendInfo(passInfoObject);
        info.CullInfo = ParseCullInfo(passInfoObject);
        return info;
    }

    PassInfo tag_invoke(const boost::json::value_to_tag<PassInfo> &, boost::json::value const &passInfoValue)
    {
        auto &passInfoObject = passInfoValue.as_object();

        PassInfo info{ParsePassInfo(passInfoObject)};

        boost::json::object openGLSources;
        if (TryGetValue(passInfoObject, "opengl", openGLSources))
        {
            for (int i = 0; i < ShaderLoaderUtils::SUPPORTED_SHADERS_COUNT; ++i)
            {
                TryGetValue(openGLSources, SHADER_NAMES[i], info.OpenGLShaderSourcePaths[i]);
            }
        }

        TryGetValue(passInfoObject, "metal", info.MetalShaderSourcePath);

        TryGetValue(passInfoObject, "colorFormat", reinterpret_cast<int&>(info.ColorFormat), -1);
        TryGetValue(passInfoObject, "depthFormat", reinterpret_cast<int&>(info.DepthFormat), -1);

        TryGetValue(passInfoObject, "vertexAttributes", info.VertexAttributes);

        return info;
    }

    void Parse(const std::string &shaderSource, std::vector<PassInfo> &passes, std::unordered_map<std::string, std::string> &properties)
    {
        auto shaderInfoObject = boost::json::parse(shaderSource).as_object();
        TryGetValue(shaderInfoObject, "passes", passes);
        TryGetValue(shaderInfoObject, "properties", properties);
    }
}
