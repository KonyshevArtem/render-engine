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
      "zWrite": false,
      "blend": {
        "SrcFactor": "SrcAlpha",
        "DstFactor": "OneMinusSrcAlpha"
      },
      "vertex": "standard.vs",
      "fragment": "standard.fs"
    }
  ]
}
