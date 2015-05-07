#include "hitbox.h"


hitbox_t hitbox_t::get_aabb() const
{

	glm::vec3 min = this->points[0];
	glm::vec3 max = this->points[0];

	for(const auto& p : this->points)
	{
		for(int i = 0; i < 3; i++)
		{
			min[i] = std::min(min[i], p[i]);
			max[i] = std::max(max[i], p[i]);
		}
	}
	
	auto ret = hitbox_t::box(max - min);
	ret.pos = (min + max) / 2.0f;

	return ret;
}


hitbox_t hitbox_t::box(glm::vec3 dim) 
{
	hitbox_t ret;

	dim /= 2.0f;

	glm::ivec3 mult;
	for(mult.x = -1; mult.x <= 1; mult.x += 2)
	{
		for(mult.y = -1; mult.y <= 1; mult.y += 2)
		{
			for(mult.z = -1; mult.z <= 1; mult.z += 2)
			{
				glm::vec3 push;
				for(int i = 0; i < 3; i++)
				{
					push[i] = mult[i] * dim[i];
				}
				ret.points.push_back(push);
			}
		}
	}
	
	ret.axes.push_back(axis::x());
	ret.axes.push_back(axis::y());
	ret.axes.push_back(axis::z());
	return ret;
}

float greatest_along(const std::vector<glm::vec3>& points, glm::vec3 axis)
{
	float ret = 0.0f;
	for(auto p : points)
	{
		ret = std::max(ret, abs(glm::dot(p, axis)));
	}
	return ret;
}

collision_r hit_test(hitbox_t lhs, hitbox_t rhs)
{
	collision_r result;
	result.collided = false;
	auto l_rot = try_rotate(lhs.rotation);
	for(auto& p : lhs.points) 
	{
		p = l_rot * p;
	}
	for(auto& axis : lhs.axes)
	{
		axis = l_rot * axis;
	}

	auto r_rot = try_rotate(rhs.rotation);
	for(auto& p : rhs.points)
	{
		p = r_rot * p;
	}
	for(auto& axis : rhs.axes)
	{
		axis = r_rot * axis;
	}

	std::vector<glm::vec3> test_axes = lhs.axes;
	test_axes.insert(test_axes.end(), rhs.axes.begin(), rhs.axes.end());
	for(auto l_axis : lhs.axes)
	{
		for(auto r_axis : rhs.axes)
		{
			if(glm::dot(l_axis, r_axis) < 0.995f)
			{
				test_axes.push_back(glm::normalize(glm::cross(l_axis, r_axis)));
			}
		}
	}

	auto center_diff = lhs.pos - rhs.pos;
	bool first = true;
	for(auto test_axis : test_axes)
	{
		auto dist = std::abs(glm::dot(center_diff, test_axis));
		auto l_rad = greatest_along(lhs.points, test_axis);
		auto r_rad = greatest_along(rhs.points, test_axis);

		auto overlap = l_rad + r_rad - dist;
		if(overlap >= 0) 
		{
			if(first || overlap < glm::length(result.mtv))
			{
				first = false;
				result.mtv = overlap * test_axis;
				result.axis = test_axis;
			}
		} else
		{
			return result;
		}
	}
	
	result.collided = true;
	return result;
}