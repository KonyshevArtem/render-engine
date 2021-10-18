uniform sampler2D _Albedo;
uniform vec4 _AlbedoST;

#ifdef _SMOOTHNESS
uniform float _Smoothness;
#endif

#ifdef _NORMAL_MAP
uniform sampler2D _NormalMap;
uniform vec4 _NormalMapST;
#endif