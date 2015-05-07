#pragma once
#include "glm_helper.h"
#include <vector>
struct hitbox_t {
	glm::vec3 pos;
	glm::vec3 rotation;
	std::vector<glm::vec3> axes;
	std::vector<glm::vec3> points;
	bool checked;

	hitbox_t() : checked(false) {};

	static hitbox_t box(glm::vec3 dim);
	hitbox_t get_aabb() const;
};



struct collision_r {
	bool collided;
	glm::vec3 mtv;
	glm::vec3 axis;
};

collision_r hit_test(hitbox_t, hitbox_t);