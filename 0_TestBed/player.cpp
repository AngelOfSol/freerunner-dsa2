#include "player.h"
#include "glm_helper.h"

template <class T>
void update_physics(T& obj, float time_step)
{
	obj.vel += obj.accel * time_step;
	obj.pos += obj.vel * time_step;
}

void player_t::update(float time_step)
{
	auto forward_dir = glm::vec3(this->facing.dir().x, 0.0f, this->facing.dir().z);
	forward_dir = glm::normalize(forward_dir);
	this->accel = this->facing.accel * forward_dir + accel.y * axis::y();

	update_physics(*this, time_step);
	
	auto y_vel = axis::y(this->vel.y);
	auto xz_vel = axis::x(this->vel.x) + axis::z(this->vel.z);
	auto vel_val = std::min(0.35f, glm::length(xz_vel));
	auto vel_dir = vel_val == 0 ? glm::vec3() : glm::normalize(xz_vel);
	xz_vel = vel_val * vel_dir;

	this->vel = y_vel + xz_vel;

}