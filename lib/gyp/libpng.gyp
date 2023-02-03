{
  "targets": [{
    "target_name": "libpng",
    "type": 'static_library',
    "sources": [
      '../libpng-1.6.39/png.c',
      '../libpng-1.6.39/pngerror.c',
      '../libpng-1.6.39/pngget.c',
      '../libpng-1.6.39/pngmem.c',
      '../libpng-1.6.39/pngpread.c',
      '../libpng-1.6.39/pngread.c',
      '../libpng-1.6.39/pngrio.c',
      '../libpng-1.6.39/pngrtran.c',
      '../libpng-1.6.39/pngrutil.c',
      '../libpng-1.6.39/pngset.c',
      '../libpng-1.6.39/pngtrans.c',
      '../libpng-1.6.39/pngwio.c',
      '../libpng-1.6.39/pngwrite.c',
      '../libpng-1.6.39/pngwtran.c',
      '../libpng-1.6.39/pngwutil.c',
      '../libpng-1.6.39/arm/arm_init.c',
      '../libpng-1.6.39/arm/filter_neon_intrinsics.c',
      '../libpng-1.6.39/arm/palette_neon_intrinsics.c',
    ],
    "dependencies": ['zlib.gyp:zlib'],
  }]
}