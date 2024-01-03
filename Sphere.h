#pragma once

#include "Hittable.h"
#include "Ray.h"

class Sphere : public Hittable
{
public:
	Sphere(glm::vec3 _center, float _radius, shared_ptr<Material> _material)
        : center(_center), radius(_radius), mat(_material) {};
	
	bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override 
	{
        // a=dir*dir; b=2dir*(orig-center); c=(orig-center)*(orig-center)-radisu^2
        glm::vec3 oc = r.origin() - center;
        auto a = glm::dot(r.direction(), r.direction());
        auto b = 2.0f * glm::dot(r.direction(), oc);
        auto c = glm::dot(oc, oc) - radius * radius;
        auto discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // find the nearest root that lies in the acceptable range
        auto root = (-b - sqrtd) / (2.0f * a);
        if (!ray_t.surrounds(root))
        {
            root = (-b + sqrtd) / (2.0f * a);
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        glm::vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
	}


private:
	glm::vec3 center;
	float radius;
    shared_ptr<Material> mat;
};

