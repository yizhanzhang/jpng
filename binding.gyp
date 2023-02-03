{
  "targets": [
    {
      "target_name": "main",
      "sources": [
        "src/main.cpp",
        "src/image.cpp",
        "src/util.cpp"
      ],
      "dependencies": ["lib/gyp/libpng.gyp:libpng"],
      "include_dirs": ["lib/libpng-1.6.39"]
    }
  ]
}