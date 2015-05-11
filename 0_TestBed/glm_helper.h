#pragma once
#include "graphics.h"
#include <vector>
glm::mat3 try_rotate(glm::vec3 axis) ;

glm::vec2 to_xz(glm::vec3 v);

glm::vec3 from_xz(glm::vec2 v);

glm::vec2 to_yz(glm::vec3 v);

glm::vec3 from_yz(glm::vec2 v);

std::ostream& operator <<(std::ostream& out, glm::vec2& v);
std::ostream& operator <<(std::ostream& out, glm::vec3& v);

std::vector<glm::vec3> octant_dirs();


namespace axis 
{
	glm::vec3 x();
	glm::vec3 y();
	glm::vec3 z();
	
	template <typename T>
	glm::vec3 x(T x) 
	{
		glm::vec3 axis(0.0f);
		axis.x = static_cast<float>(x);
		return axis;
	}
	template <typename T>
	glm::vec3 y(T y) 
	{
		glm::vec3 axis(0.0f);
		axis.y = static_cast<float>(y);
		return axis;
	}
	template <typename T>
	glm::vec3 z(T z) 
	{
		glm::vec3 axis(0.0f);
		axis.z = static_cast<float>(z);
		return axis;
	}
}