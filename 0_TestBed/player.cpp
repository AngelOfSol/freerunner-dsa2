#include "player.h"


template <class T>
void update_physics(T& obj, float time_step)
{
	obj.vel += obj.accel * time_step;
	obj.pos += obj.vel * time_step;
}

void player_t::update(float time_step)
{
	glm::vec3 forward_dir(this->facing.dir.x, 0.0f, this->facing.dir.z);
	forward_dir = glm::normalize(forward_dir);
	this->accel = this->facing.accel * forward_dir + glm::vec3(0.0f, accel.y, 0.0f);

	update_physics(*this, time_step);
	
	auto y_vel = glm::vec3(0.0f, this->vel.y, 0.0f);
	auto xz_vel = glm::vec3(this->vel.x, 0.0f, this->vel.z);
	auto vel_val = std::min(0.35f, glm::length(xz_vel));
	auto vel_dir = vel_val == 0 ? glm::vec3() : glm::normalize(xz_vel);
	xz_vel = vel_val * vel_dir;

	this->vel = y_vel + xz_vel;

}