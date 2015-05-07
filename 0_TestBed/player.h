#pragma once
#include "graphics.h"
#include "hitbox.h"
#include "summable_ring_buffer.h"

struct player_t
{
	glm::vec3 pos;
	glm::vec3 vel; 
	glm::vec3 accel;

	ring_buffer_t<30> cumulative_turn_angle;

	bool in_air;

	struct facing_t
	{
		glm::vec2 rot;
		glm::vec3 dir() const 
		{
			return glm::rotateY(glm::rotateX(glm::vec3(0.0f, 0.0f, 1.0f), this->rot.x), this->rot.y);
		};
		glm::mat4 transform() const {
			return glm::rotate(this->rot.y, axis::y()) * glm::rotate(this->rot.x, axis::x());	
		};
		float accel;

		facing_t()
			: accel(0.0f), rot(0.0f, 0.0f) { };
	} facing;

	player_t()
		: pos(), vel(), accel(){ };

	void update(float time_step);
};