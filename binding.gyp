{
  "targets": [
    {
      "target_name": "main",
      "sources": [
        "src/main.cpp",
        "src/image.cpp",
        "src/util.cpp"
      ],
      "dependencies": [
        "lib/gyp/libpng.gyp:libpng",
        "lib/gyp/libjpeg-turbo.gyp:libjpeg"
      ],
      "include_dirs": [
        "lib/libpng-1.6.39",
        "lib/libjpeg-turbo"
      ]
    }
  ]
}