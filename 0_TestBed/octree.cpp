#include "octree.h"


collision_r octree_node::check_collisions(hitbox_t check_with)
{
	collision_r ret;
	ret.collided = false;



	if(this->children.is_none())
	{
		for(auto& c : this->contained)
		{
			if(!c->checked)
			{
				c->checked = true;
				auto test = hit_test(*c, check_with);
				if(test.collided == true)
				{
					return test;
				}
			}
		}
	} else {
		for(auto& c : *this->children.unwrap())
		{
			auto results = c.check_collisions(check_with);
			if(results.collided)
			{
				return results;
			}
		}
	}

	return ret;
}

octree_node create_octree(glm::vec3 center, glm::vec3 dims, std::vector<hitbox_t>& elements, int depth)
{
	octree_node ret;
	int hit_count = 0;

	auto this_box = hitbox_t::box(dims);
	this_box.pos = center;
	ret.area = this_box;
	for(auto& e : elements)
	{
		auto result = hit_test(this_box, e);
		if(result.collided)
		{
			hit_count++;
			ret.contained.push_back(&e);
		}
	}
	if(hit_count > 2 && depth < 6)
	{
		auto new_dims = dims / 2.0f;
		auto new_offsets = dims / 4.0f;


		ret.contained.clear();

		ret.children = make_option(std::make_shared<std::array<octree_node, 8>>());
		
		auto& children = *ret.children.unwrap();
		int child_num = 0;
		for(auto dir : octant_dirs())
		{
			children[child_num] = create_octree(center + dir * new_offsets, new_dims, elements, depth + 1);
			child_num++;
		}
	}

	return ret;
}