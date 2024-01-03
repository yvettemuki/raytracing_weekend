#include "ImageHandler.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void ImageHandler::WriteToPNG(char const* filename, int w, int h, const void* data)
{
	stbi_flip_vertically_on_write(0);
	if (stbi_write_png(filename, w, h, 3, data, 3 * w))
	{
		std::cout << "save to " << filename << std::endl;
	}
	else
	{
		std::cout << "error" << std::endl;
	}
}

//unsigned int* ImageHandler::ReadFromPNG(char const* filename, int* w, int* h, int* n_ch)
//{
//	stbi_set_flip_vertically_on_load(1);
//	unsigned char* data = stbi_load(filename, w, h, n_ch, 0);
//
//	// convert the unsigned char* to unsigned int*
//	int width = *w;
//	int height = *h;
//	int byte_per_pix = *n_ch;
//	unsigned int* pixels = new unsigned int[width * height];
//	for (int v = 0; v < height; v++)
//	{
//		for (int u = 0; u < width; u++)
//		{
//			unsigned char* pixel_offset = data + (v * width + u) * byte_per_pix;
//			unsigned int r = (unsigned int)pixel_offset[0];
//			unsigned int g = (unsigned int)pixel_offset[1];
//			unsigned int b = (unsigned int)pixel_offset[2];
//			unsigned int a = (unsigned int)pixel_offset[3];
//			pixels[v * width + u] = rgb2Hex(r, g, b, a);
//		}
//	}
//
//	return pixels;
//}
