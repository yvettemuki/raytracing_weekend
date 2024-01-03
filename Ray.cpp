#include "Ray.h"

glm::vec3 Ray::origin() const
{
	return orig;
}

glm::vec3 Ray::direction() const
{
	return dir;
}

glm::vec3 Ray::at(float t) const
{
	return orig + dir*t;
}