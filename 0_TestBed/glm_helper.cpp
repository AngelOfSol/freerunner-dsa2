#include "glm_helper.h"

glm::mat3 try_rotate(glm::vec3 axis) 
{
	auto len = glm::length(axis);
	if (len == 0)
		return glm::mat3(1.0f);
	return glm::mat3(glm::rotate(len, axis));
}
glm::vec2 to_xz(glm::vec3 v)
{
	return glm::vec2(v.x, v.z);
}
glm::vec3 from_xz(glm::vec2 v)
{
	return glm::vec3(v.x, 0.0f, v.y);
}

glm::vec2 to_yz(glm::vec3 v)
{
	return glm::vec2(v.y, v.z);
}
glm::vec3 from_yz(glm::vec2 v)
{
	return glm::vec3(0.0f, v.x, v.y);
}

std::ostream& operator <<(std::ostream& out, glm::vec2& v)
{
	out << v.x << " " << v.y;
	return out;
}


std::ostream& operator <<(std::ostream& out, glm::vec3& v)
{
	out << "(" << v.x << " " << v.y << " " << v.z << ")";
	return out;
}


namespace axis 
{
	glm::vec3 x() 
	{
		glm::vec3 axis(0.0f);
		axis.x = 1.0f;
		return axis;
	}
	glm::vec3 y() 
	{
		glm::vec3 axis(0.0f);
		axis.y = 1.0f;
		return axis;
	}
	glm::vec3 z() 
	{
		glm::vec3 axis(0.0f);
		axis.z = 1.0f;
		return axis;
	}
	
}


std::vector<glm::vec3> octant_dirs()
{
	std::vector<glm::vec3> ret;
	glm::ivec3 mult;
	for(mult.x = -1; mult.x <= 1; mult.x += 2)
	{
		for(mult.y = -1; mult.y <= 1; mult.y += 2)
		{
			for(mult.z = -1; mult.z <= 1; mult.z += 2)
			{
				ret.push_back(glm::vec3(mult));
			}
		}
	}
	return ret;
}