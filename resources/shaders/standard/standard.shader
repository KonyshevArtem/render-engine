{
  "default": {
    "_Albedo": "white",
    "_NormalMap": "normal",
    "_SpecularMask": "white",
    "_ReflectionCube": "black",
    "_ReflectionMask": "white",
    "_SmoothnessMask": "white",
    "_MetallnessMask": "white"
  },
  "passes": [
    {
      "tags": {
        "LightMode": "Forward"
      },
      "vertex": "standard.vs",
      "fragment": "standard.fs"
    },
    {
      "tags": {
        "LightMode": "ShadowCaster"
      },
      "vertex": "../shadowCaster/shadowCaster.vs",
      "fragment": "../shadowCaster/shadowCaster.fs"
    }
  ]
}