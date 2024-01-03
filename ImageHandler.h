#pragma once
class ImageHandler
{
public:
	static void WriteToPNG(char const* filename, int w, int h, const void* data);
	unsigned int* ReadFromPNG(char const* filename, int* w, int* h, int* n_ch);
};

