{
  "properties": {
    "_Albedo": "white",
    "_NormalMap": "normal",
    "_ReflectionCube": "black",
    "_Data": "white"
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