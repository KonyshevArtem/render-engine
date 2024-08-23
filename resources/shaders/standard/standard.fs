uniform sampler2D _Albedo;

#ifdef _DATA_MAP
uniform sampler2D _Data;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
#endif

#ifdef _REFLECTION
uniform samplerCube _ReflectionCube;
#endif

#ifdef _RECEIVE_SHADOWS
uniform sampler2D _DirLightShadowMap;
uniform sampler2DArray _SpotLightShadowMapArray;
uniform sampler2DArray _PointLightShadowMapArray;
#endif

#include "standard_shared.h"

DECLARE_UBO(CameraData);
DECLARE_UBO(PerMaterialData);
DECLARE_UBO(Lighting);
DECLARE_UBO(Shadows);

in Varyings vars;
out vec4 outColor;

void main(){
    outColor = fragmentFunction(vars, CameraDataVar, PerMaterialDataVar, LightingVar,
#ifdef _NORMAL_MAP
        _NormalMap,
#endif
#ifdef _DATA_MAP
        _Data,
#endif
#ifdef _REFLECTION
        _ReflectionCube,
#endif
#ifdef _RECEIVE_SHADOWS
        ShadowsVar,
        _DirLightShadowMap,
        _SpotLightShadowMapArray,
        _PointLightShadowMapArray,
#endif
        _Albedo);
}