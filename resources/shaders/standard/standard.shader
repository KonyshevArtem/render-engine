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
      "opengl":
      {
        "vertex": "standard.vs",
        "fragment": "standard.fs"
      },
      "metal": "standard.metal"
    }
  ]
}