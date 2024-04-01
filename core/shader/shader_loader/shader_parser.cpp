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
                {"None",  static_cast<CullFace>(0)},
                {"Front", CullFace::FRONT},
                {"Back",  CullFace::BACK},
                {"Both",  CullFace::FRONT_AND_BACK},
        };

namespace ShaderParser
{

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
        CullInfo info{true, CullFace::BACK};

        std::string cullValue;
        if (TryGetValue(passInfoObject, "cull", cullValue))
        {
            info.Face = StringToEnum(cullValue, CULL_FACE_MAP);
            info.Enabled = static_cast<int>(info.Face) != 0;
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

        return info;
    }

    void Parse(const std::string &shaderSource, std::vector<PassInfo> &passes, std::unordered_map<std::string, std::string> &properties)
    {
        auto shaderInfoObject = boost::json::parse(shaderSource).as_object();
        TryGetValue(shaderInfoObject, "passes", passes);
        TryGetValue(shaderInfoObject, "properties", properties);
    }
}
