{
  "default": {
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
      "zWrite": false,
      "blend": {
        "SrcFactor": "SrcAlpha",
        "DstFactor": "OneMinusSrcAlpha"
      },
      "opengl":
      {
        "vertex": "standard.vs",
        "fragment": "standard.fs"
      },
      "metal": "standard.metal"
    }
  ]
}
