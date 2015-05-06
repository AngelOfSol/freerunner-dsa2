#pragma once
#include "graphics.h"

glm::vec2 to_xz(glm::vec3 v)
{
	return glm::vec2(v.x, v.z);
}
glm::vec3 from_xz(glm::vec2 v)
{
	return glm::vec3(v.x, 0.0f, v.y);
}

std::ostream& operator <<(std::ostream& out, glm::vec2& v)
{
	out << v.x << " " << v.y;
	return out;
}
