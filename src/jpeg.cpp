#include "jpeglib.h"
#include "jerror.h"

Result decodeJpegBuffer(Image& img, CompressData& inputData) {
  Result result;

  jpeg_decompress_struct cinfo;
  jpeg_error_mgr jerror;
  cinfo.err = jpeg_std_error(&jerror);

  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, inputData.buffer, inputData.length);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.out_color_space = JCS_EXT_RGBA;
  jpeg_start_decompress(&cinfo);
  img.width = cinfo.image_width;
  img.height = cinfo.image_height;
  img.mallocImageData();
  int line;
  while((line = cinfo.output_scanline) < (int)img.height){
    jpeg_read_scanlines(&cinfo, &img.data[line], 1);
	}

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  return result;
}

Result encodeJpegBuffer(Image& img, CompressData& outputData) {
  Result result;

  jpeg_compress_struct cinfo;
  jpeg_error_mgr jerror;
  cinfo.err = jpeg_std_error(&jerror);

  jpeg_create_compress(&cinfo);
  jpeg_mem_dest(&cinfo, &outputData.buffer, &outputData.length);
  cinfo.image_width = img.width;
	cinfo.image_height = img.height;
	cinfo.input_components = 4;
	cinfo.in_color_space = JCS_EXT_RGBA;
  jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 90, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
  int line;
  while((line = cinfo.next_scanline) < (int)img.height){
    jpeg_write_scanlines(&cinfo, &img.data[line], 1);
	}

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  return result;
}