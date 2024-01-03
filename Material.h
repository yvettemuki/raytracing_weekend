#pragma once

#include "Utilities.h";
#include "Hittable.h"

class hit_record;

class Material
{
public:
	virtual ~Material() = default;

	virtual bool scatter(
		const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered
	) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const glm::vec3 a) : albedo(a) {}

	bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered)
	const override {
		auto scatter_direction = rec.normal + random_unit_vector();

		if (near_zero(scatter_direction))
			scatter_direction = rec.normal;

		scattered = Ray(rec.p, scatter_direction);
		attenuation = albedo;  // can assign probability p
		return true;
	}


private:
	glm::vec3 albedo;
};

class Metal : public Material
{
public:
	Metal(const glm::vec3 a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered)
	const override {
		glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * random_unit_vector());
		attenuation = albedo;
		return (glm::dot(scattered.direction(), rec.normal) > 0);
	}

private:
	glm::vec3 albedo;
	float fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(float index_of_refraction) : ir(index_of_refraction) {}

	bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered)
	const override {
		attenuation = glm::vec3(1.0, 1.0, 1.0);  // for refraction, not ray be absorbed
		float refraction_ratio = rec.front_face ? (1.0f / ir) : ir;

		glm::vec3 unit_view_direction = glm::normalize(r_in.direction());
		float cos_theta = fmin(dot(-unit_view_direction, rec.normal), 1.0);
		float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

		// in case of total internal reflection
		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		glm::vec3 direction;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
			direction = reflect(unit_view_direction, rec.normal);
		else
			direction = refract(unit_view_direction, rec.normal, refraction_ratio);
		
		scattered = Ray(rec.p, direction);
		return true;
	}

private:
	float ir;  // index of refraction

	static float reflectance(float cosine, float ref_idx)
	{
		// Use Schlick's approximation for reflectance
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow(1 - cosine, 5);
	}
};