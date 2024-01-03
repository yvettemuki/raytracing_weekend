#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "ImageHandler.h"
#include "Interval.h";

using namespace std;

class Framebuffer
{
public:
	vector<glm::u8vec3> texData;
	int w, h;

	Framebuffer() {
		w = 256;
		h = 256;
		texData.resize(w * h, glm::u8vec3(255, 167, 50));
	}

	Framebuffer(int _w, int _h) : w(_w), h(_h) {
		texData.resize(w * h, glm::u8vec3(255, 167, 50));
	}

	void write_color(int u, int v, glm::vec3 col, int samples_per_pixel)
	{
		auto r = col.x;
		auto g = col.y;
		auto b = col.z;

		// divide the color by the number of samples
		auto scale = 1.0 / samples_per_pixel;
		r *= scale;
		g *= scale;
		b *= scale;

		// apply the linear to gamma transform
		r = linear_to_gamma(r);
		g = linear_to_gamma(g);
		b = linear_to_gamma(b);

		static const Interval intensity(0.000, 0.999);
		int ir = static_cast<int>(256* intensity.clamp(r));
		int ig = static_cast<int>(256 * intensity.clamp(g));
		int ib = static_cast<int>(256 * intensity.clamp(b));

		texData[v * w + u] = glm::u8vec3(ir, ig, ib);
	}

	void saveToPNG(char const* filename)
	{
		ImageHandler::WriteToPNG(filename, w, h, static_cast<void*>(texData.data()));
	}

	~Framebuffer() = default;

};
