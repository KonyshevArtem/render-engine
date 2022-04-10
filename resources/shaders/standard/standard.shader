{
  "default": {
    "_Albedo": "white",
    "_NormalMap": "normal",
    "_SpecularMask": "white"
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