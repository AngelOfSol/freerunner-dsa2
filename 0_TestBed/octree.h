#pragma once
#include <memory>
#include "glm_helper.h"
#include "hitbox.h"
#include "option.h"
#include <array>
#include <vector>
struct octree_node
{
	option_t<std::shared_ptr<std::array<octree_node, 8>>> children;
	std::vector<hitbox_t *> contained;

	glm::vec3 center;
	glm::vec3 dims;

	collision_r check_collisions(hitbox_t);
	hitbox_t area;
};

octree_node create_octree(glm::vec3 center, glm::vec3 dims, std::vector<hitbox_t>& elements, int depth = 0);