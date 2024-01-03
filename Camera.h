#pragma once

#include "Utilities.h"
#include "Hittable.h"
#include "Framebuffer.h"
#include "Material.h"

#include <iostream>


class Camera
{
public:
	float aspect_ratio = 1.0;
	int image_width, image_height;
	float focal_length;
	float viewport_width, viewport_height;
	glm::vec3 center;
	int samples_per_pixel = 10; // count of random samples for each pixel
	int max_depth = 10;         // maximum number of ray bounces into scene

	float vfov = 90;
	glm::vec3 lookfrom = glm::vec3(0, 0, -1);
	glm::vec3 lookat = glm::vec3(0, 0, 0);
	glm::vec3 vup = glm::vec3(0, 1, 0);

	float defocus_angle = 0; // variation angle of rays through each pixel
	float focus_dist = 10; // distance from camera center to plane of perfect focus


	void render(Framebuffer* buf, const Hittable& world)
	{
		initialize();

		for (int j = 0; j < image_height; j++)
		{
			float scanning_percent = static_cast<int>((float)j / image_height * 100);
			std::clog << "\rScanlines progress: " << scanning_percent << "%" << std::flush;
			for (int i = 0; i < image_width; i++)
			{
				glm::vec3 pixel_color(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++)
				{
					Ray r = get_ray(i, j);
					pixel_color += ray_color(r, max_depth, world);
				}

				buf->write_color(i, j, pixel_color, samples_per_pixel);
			}
		}

		std::clog << "\rDone.                 \n";
	}

private:

	glm::vec3 pixel00_loc;   
	glm::vec3 pixel_delta_u;  
	glm::vec3 pixel_delta_v;  
	glm::vec3 u, v, w;
	glm::vec3 defocus_disk_u;
	glm::vec3 defocus_disk_v;

	Ray get_ray(int i, int j) const
	{
		// get a randomly sample ray for the pixel at loc i, j (non-focus-blur)
		// get a randomly-sample camera ray fro the pixel at location i,j, originating from the camera defocus disk;
		
		auto pixel_center = pixel00_loc + (float)i * pixel_delta_u + (float)j * pixel_delta_v;
		auto pixel_sample = pixel_center + pixel_sample_square();

		//auto ray_origin = center;
		auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
		auto ray_direction = pixel_sample - ray_origin;

		return Ray(ray_origin, ray_direction);
	}

	glm::vec3 pixel_sample_square() const
	{
		// return a random point in the square surrounding a pixel at the origin
		auto px = -0.5f + random_float();
		auto py = -0.5f + random_float();
		return (px * pixel_delta_u) + py * (pixel_delta_v);
	}

	// init with default parameters
	void initialize()
	{
		center = lookfrom;

		//focal_length = glm::length(lookfrom - lookat);
		auto theta = deg2Rad(vfov);
		auto h = tanf(theta / 2.0f);
		viewport_height = 2 * h * focus_dist;  // focal_length
		aspect_ratio = static_cast<float>(image_width) / image_height;
		viewport_width = viewport_height * aspect_ratio;

		// Calculate the u,v,w unit basis vectors for the camera coodinate frame;
		w = glm::normalize(lookfrom - lookat);
		u = glm::normalize(glm::cross(vup, w));
		v = glm::cross(w, u);

		// calc the vector u and v
		glm::vec3 viewport_u = viewport_width * u;
		glm::vec3 viewport_v = viewport_height * -v;

		// calc the delta u and v vectors
		pixel_delta_u= viewport_u / static_cast<float>(image_width);
		pixel_delta_v = viewport_v / static_cast<float>(image_height);

		// calc the location of the upper left pixel
		glm::vec3 viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0f - viewport_v / 2.0f;  //focal_length
		pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

		// calcualte the camera defocus disk basis vectors
		auto defocus_radius = focus_dist * tanf(deg2Rad(defocus_angle / 2));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	glm::vec3 defocus_disk_sample() const
	{
		// return a random point in the camer defocus disk
		auto p = random_in_unit_disk();
		return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
	}

	glm::vec3 ray_color(const Ray& r, int depth, const Hittable& world) const
	{
		hit_record rec;

		if (depth <= 0)
			return glm::vec3(0, 0, 0);

		if (world.hit(r, Interval(0.001, INF), rec))
		{
			Ray scattered;
			glm::vec3 attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, depth - 1, world);
			return glm::vec3(0, 0, 0);
		}

		glm::vec3 unit_direction = glm::normalize(r.direction());
		float a = 0.5f * (unit_direction.y + 1.0);
		return (1.0f - a) * glm::vec3(1, 1, 1) + a * glm::vec3(0.5, 0.7, 1.0);
	}
};
