#pragma once

#include "Hittable.h";
#include "Interval.h";

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HittableList : public Hittable
{
public:
	std::vector<shared_ptr<Hittable>> objects;

	HittableList() {}
	HittableList(shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<Hittable> object)
	{
		objects.push_back(object);
	}

	bool hit(const Ray& r, Interval ray_t, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool hit_anything = false;
		auto closest_so_far = ray_t.max;  // z-test

		for (const auto& object : objects)
		{
			if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}
	


};

