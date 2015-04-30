#pragma once
#include "graphics.h"
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
		glm::vec3 dir;
		glm::vec3 side;
		float accel;

		facing_t()
			: dir(0.0f, 0.0f, 1.0f), accel(0.0f), side(1.0f, 0.0f, 0.0f) { };
	} facing;

	player_t()
		: pos(), vel(), accel() { };

	void update(float time_step);
};