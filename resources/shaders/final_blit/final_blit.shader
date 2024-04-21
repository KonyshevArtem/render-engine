{
  "passes": [
    {
      "zWrite": false,
      "zTest": "Always",
      "opengl":
      {
        "vertex": "final_blit.vs",
        "fragment": "final_blit.fs"
      },
      "metal": "final_blit.metal",
      "vertexAttributes": [
        {
          "index": 0,
          "dimensions": 3,
          "dataType": 7,
          "isNormalized": false,
          "stride": 12,
          "offset": 0
        }
      ]
    }
  ]
}
