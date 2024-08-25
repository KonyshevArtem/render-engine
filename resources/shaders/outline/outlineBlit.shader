{
  "passes": [
    {
      "zWrite": false,
      "zTest": "Always",
      "opengl": {
        "vertex": "outlineBlit.vs",
        "fragment": "outlineBlit.fs"
      },
      "metal": "outlineBlit.metal"
    },
    {
      "zWrite": false,
      "zTest": "Always",
      "opengl": {
        "vertex": "silhouette.vs",
        "fragment": "silhouette.fs"
      },
      "metal": "silhouette.metal"
    }
  ]
}