#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

// Usings 

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const float INF = std::numeric_limits<float>::infinity();
const float PI = 3.1415926535;

// Utility Functions

inline float deg2Rad(float degs)
{
	return degs * PI / 180.0f;
}

inline float random_float()
{
	return rand() / (RAND_MAX + 1.0);
}

inline float random_float(float min, float max)
{
	return min + (max - min) * random_float();
}

static glm::vec3 random()
{
	return glm::vec3(random_float(), random_float(), random_float());
}

static glm::vec3 random(float min, float max)
{
	return glm::vec3(random_float(min, max), random_float(min, max), random_float(min, max));
}

inline glm::vec3 random_in_unit_sphere()
{
	while (true)
	{
		glm::vec3 p = random(-1, 1);
		if (glm::length2(p) < 1)
			return p;
	}
}

inline glm::vec3 random_unit_vector()
{
	return glm::normalize(random_in_unit_sphere());
}

inline glm::vec3 random_on_hemisphere(const glm::vec3& normal)
{
	glm::vec3 on_unit_sphere = random_unit_vector();
	if (glm::dot(on_unit_sphere, normal) > 0.0)
		return on_unit_sphere;
	else
		return -on_unit_sphere;
}

inline float linear_to_gamma(float linear_component)
{
	return sqrt(linear_component);
}

inline bool near_zero(const glm::vec3 val)
{
	auto s = 1e-8;
	return (fabs(val.x) < s) && (fabs(val.y) < s) && (fabs(val.z) < s);
}

inline glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n)
{
	return v - 2 * glm::dot(v, n) * n;
}

inline glm::vec3 refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat)
{
	// R'_perpendicular = etai_over_etat * unit_incident_ray + dot(-unit_incient_ray, n) * n)
	// R'_parallel = -sqrt(1 - fabs(R'_perpendicular)^2) * n
	float cos_theta = fmin(glm::dot(-uv, n), 1.0f);
	glm::vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
	glm::vec3 r_out_parallel = -sqrt(fabs(1.0f - glm::length2(r_out_perp))) * n;
	return r_out_perp + r_out_parallel;
}

inline glm::vec3 random_in_unit_disk()
{
	while (true)
	{
		auto p = glm::vec3(random_float(-1, 1), random_float(-1, 1), 0);
		if (glm::length2(p) < 1)
			return p;
	}
}

// Common Headers

#include "Interval.h"
#include "Ray.h"
