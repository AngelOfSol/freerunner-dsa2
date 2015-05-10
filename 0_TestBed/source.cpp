#include "player.h"
#include "glm_helper.h"
#include "hitbox.h"
#include "octree.h"
#include <string>
#include <iostream>
#include <fstream>

LRESULT CALLBACK window_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Callbacks for winapp 
    switch(msg)
    {
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

struct model_t
{
	glm::vec3 pos;
	glm::vec3 rotate;
	glm::vec3 scale;

	glm::mat4 get_transform() const {
		auto rot_angle = glm::length(rotate);
		auto rot_mat = rot_angle == 0 ? glm::mat4(1.0f) : glm::rotate(rot_angle, rotate);
		return glm::translate(this->pos) * rot_mat * glm::scale(this->scale);
	}

	model_t()
		: scale(1.0f, 1.0f, 1.0f) { };

	std::string name;
	
	hitbox_t generate_hitbox(std::vector<glm::vec3> points) const 
	{
		hitbox_t ret;

		ret.pos = this->pos;
		ret.rotation = this->rotate;

		for(unsigned int i = 0; i < points.size(); i += 3)
		{
			auto& p0 = points[i + 0];
			auto& p1 = points[i + 1];
			auto& p2 = points[i + 2];
			p0 *= this->scale;
			p1 *= this->scale;
			p2 *= this->scale;
			ret.axes.push_back(glm::normalize(glm::cross(p0 - p1, p0 - p2)));
		}
		ret.points = std::move(points);
		auto cmp_vec3 =  [](const glm::vec3& l, const glm::vec3& r) {
			if (l.x == r.x)	{
				if(l.y == r.y) {
					return l.z < r.z;
				} else {
    				return l.y < r.y;
				}
			} else {
    			return l.x < r.x;
			}
		};
		auto cmp_abs_vec3 =  [](glm::vec3 l, glm::vec3 r) {
			for(int i = 0; i < 3; i++)
			{
				l[i] = std::abs(l[i]);
				r[i] = std::abs(r[i]);
			}
			if (l.x == r.x)	{
				if(l.y == r.y) {
					return l.z < r.z;
				} else {
    				return l.y < r.y;
				}
			} else {
    			return l.x < r.x;
			}
		};
		auto eq_vec3 = [] (glm::vec3 l, glm::vec3 r) {
			return l == r || l == -r;
		};
		std::sort(ret.points.begin(), ret.points.end(), cmp_vec3);
		ret.points.erase(std::unique(ret.points.begin(), ret.points.end()), ret.points.end());
		std::sort(ret.axes.begin(), ret.axes.end(), cmp_abs_vec3);
		ret.axes.erase(std::unique(ret.axes.begin(), ret.axes.end(), eq_vec3), ret.axes.end());

		return ret;
	}
};

struct controls_t
{
	int x;
	int y;
	enum button_t {
		OFF,
		ONCE,
		ON
	} b_jump, b_x, b_y;

	glm::vec2 delta_mouse;
};

void update_with_controls(const controls_t& controls, player_t& player, float time_step)
{	
	if(!player.in_air) 
	{
		player.facing.accel = -controls.y * 0.0035f;
	} else {
		player.facing.accel = 0;
	}

	if(controls.b_jump == controls_t::ONCE && !player.in_air)
	{
		player.in_air = true;
		player.vel.y = 0.3f;
	}

	auto rot_speed_y = 0.1f;
	auto rot_speed_x = 0.05f;//std::min(0.02f, 0.1f - glm::length(player.vel) * 0.1f); // 0.04f -> 0.02f

	auto xz_dir_old = glm::normalize(to_xz(player.facing.dir()));
	
	const auto turn_angle_y = rot_speed_x * controls.delta_mouse.x * time_step; 
		//+ (controls.b_x == controls_t::ONCE ? 90.0f : 0.0f) * -controls.x; disable until can do 90.0f degrees over 1 half a second
	const auto turn_angle_x = rot_speed_y * -controls.delta_mouse.y * time_step;

	player.cumulative_turn_angle.push(turn_angle_y);

	player.facing.rot.x += turn_angle_x;

	player.facing.rot.x = std::max(std::min(player.facing.rot.x, 85.0f), -85.0f);

	player.facing.rot.y += turn_angle_y;

	// applies friction along the ground 
	// and makes turning too fast slow you down
	if(!player.in_air)
	{
		auto xz_dir_new = glm::normalize(to_xz(player.facing.dir()));
		auto xz_vel_old = to_xz(player.vel);
	
		auto turn_multiplier = 1.0f;
		if(std::abs(player.cumulative_turn_angle.sum()) > 65.0f)
		{
			player.cumulative_turn_angle.clear();
			turn_multiplier = 0.25f;
		} else if(glm::length(player.vel) != 0 && glm::dot(xz_dir_new, glm::normalize(to_xz(player.vel))) < 0.8)
		{
			turn_multiplier = 0.1f;
		}

		auto friction = (controls.b_y == controls_t::OFF) ? 0.975f : 1;

		auto xz_vel_new = xz_dir_new * turn_multiplier * friction * glm::length(xz_vel_old) ;

		player.vel += from_xz(xz_vel_new - xz_vel_old);
	}
}
void physics(player_t& player)
{
	const auto gravity = -0.01f;
	player.accel.y = gravity;

	if(player.vel.y < 2 * gravity && !player.in_air)
	{
		player.in_air = true;
	}
}
void update(player_t& player, octree_node& objects, std::vector<model_t>& models, const controls_t& controls) 
{
	auto time_step = 1.0f;
	
	physics(player);

	update_with_controls(controls, player, time_step);

	player.update(time_step);



	hitbox_t player_box = hitbox_t::box(glm::vec3(1.0f, 2.0f, 1.0f));
	player_box.pos = player.pos;
	
	player_box.rotation = axis::y()  // axis
		* player.facing.rot.y; // angle in degrees


	collision_r collision_test = objects.check_collisions(player_box);
	while(collision_test.collided == true)
	{
		player.pos += collision_test.mtv;
		auto to_remove = collision_test.axis * glm::dot(collision_test.axis, player.vel);

		player.vel -= to_remove;
		if(glm::length(axis::y() - collision_test.axis) < 0.01f)
		{
			player.in_air = false;
		} else 
		{
			if(glm::length(player.vel) > 0.001f)
			{
				player.vel += glm::length(to_remove) * glm::normalize(player.vel) * 0.75f;
			}
			if(glm::dot(player.facing.dir(), collision_test.axis) < 0)
			{
				auto new_facing = player.facing.dir() - glm::dot(collision_test.axis, player.facing.dir());

				if(glm::dot(player.facing.dir(), new_facing) < 0)
					new_facing = -new_facing;

				auto temp_facing = to_xz(player.facing.dir());

				auto temp = to_xz(new_facing);

				// player.facing.rot.y = 180 / glm::pi<float>() * glm::atan2(temp.x, temp.y);
				// todo lerp new angle values
			}
		}

		collision_test = objects.check_collisions(player_box);
	}

	

	// player_box.rotation = ;
	
	while(player.facing.rot.y < -180)
	{
		player.facing.rot.y += 360; 
	}

	while(player.facing.rot.y >= 180)
	{
		player.facing.rot.y -= 360; 
	}

}
void check_controls(controls_t& ctrl, window_t& window)
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		ctrl.x = -1;
		switch(ctrl.b_x)
		{
		case controls_t::OFF:
			ctrl.b_x = controls_t::ONCE;
			break;
		default:
			ctrl.b_x = controls_t::ON;
			break;
		}
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		ctrl.x = 1;
		switch(ctrl.b_x)
		{
		case controls_t::OFF:
			ctrl.b_x = controls_t::ONCE;
			break;
		default:
			ctrl.b_x = controls_t::ON;
			break;
		}
	} else 
	{
		ctrl.x = 0;
		ctrl.b_x = controls_t::OFF;
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		ctrl.y = -1;
		switch(ctrl.b_y)
		{
		case controls_t::OFF:
			ctrl.b_y = controls_t::ONCE;
			break;
		default:
			ctrl.b_y = controls_t::ON;
			break;
		}
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		ctrl.y = 1;
		switch(ctrl.b_y)
		{
		case controls_t::OFF:
			ctrl.b_y = controls_t::ONCE;
			break;
		default:
			ctrl.b_y = controls_t::ON;
			break;
		}
	} else 
	{
		ctrl.y = 0;
		ctrl.b_y = controls_t::OFF;
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) // jump button
	{
		switch(ctrl.b_jump)
		{
		case controls_t::OFF:
			ctrl.b_jump = controls_t::ONCE;
			break;
		default:
			ctrl.b_jump = controls_t::ON;
			break;
		}
	} else 
	{
		ctrl.b_jump = controls_t::OFF;
	}

	
	POINT pt;
	tagRECT window_map;
	GetCursorPos(&pt);
	GetWindowRect(window.GetHandler(), &window_map);
	
	auto mouse = glm::ivec2(pt.x, pt.y);
	auto center = glm::ivec2((window_map.left + window_map.right) / 2, (window_map.top + window_map.bottom) / 2);
	
	ctrl.delta_mouse = glm::vec2(center - mouse);
	
	SetCursorPos(center.x, center.y);
	SetCursor(nullptr);
}
void push_updates(meshes_t& meshes, const std::vector<model_t>& models, camera_t& camera, const player_t& player)
{
	for(auto& mc : models) 
	{
		meshes.SetModelMatrix(mc.get_transform(), mc.name);
	}
	
	camera.SetPosition(player.pos + glm::vec3(0.0f, 1.4f, 0.0f));
	camera.SetForwardVector(player.facing.dir());
	camera.CalculateView();
}
void draw(meshes_t& meshes, gl_t& gl) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshes.AddInstanceToRenderList("ALL");
	meshes.Render();
	
	gl.GLSwapBuffers();
}

std::vector<std::string> split_string(std::string& string_to_split, char delimiter)
{
	std::vector<std::string> string_components;
	auto split_start = 0;
	do
	{
		auto split_end = string_to_split.find(delimiter, split_start); 

		if(split_end != std::string::npos)
		{
			string_components.push_back(string_to_split.substr(split_start, split_end - split_start));
			split_start = split_end + 1;
		}
		else
		{
			string_components.push_back(string_to_split.substr(split_start, split_end - split_start));
			split_start = std::string::npos;
		}
	} while(split_start != std::string::npos);

	return string_components;
}

glm::vec3 split_string_to_vec(std::string& string_to_split)
{
	std::vector<std::string> split;
	split = split_string(string_to_split, ';');
	return glm::vec3(std::stof(split[0]), std::stof(split[1]), std::stof(split[2]));
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	window_t window(hInstance, nCmdShow, window_callback);
	window.CreateConsoleWindow();
	window.CreateMEWindow(L"test", 1280, 720);

	auto& gl = *gl_t::GetInstance();
	gl.InitGLDevice(window.GetHandler());
	auto& meshes = *meshes_t::GetInstance();
	auto& lights = *lights_t::GetInstance();
	lights.SetPosition(glm::vec3(0, 0, 10), 1);
	lights.SetColor(glm::vec3(1, 1, 1), 1);
	lights.SetIntensity(5.0f, 1);
	lights.SetIntensity(0.75f, 0 );
	auto& camera = *camera_t::GetInstance();
	camera.SetPosition(glm::vec3());

	// --------------
	// Get the level information
	std::vector<std::string> level_components;
	std::ifstream level_file;
	level_file.open("../level1.txt");

	if(level_file.is_open())
	{
		std::string file_line;
		while (getline (level_file,file_line) )
		{
			level_components.push_back(file_line);
		}
	}
	else
	{
		std::cout<<"Could not open file";
	}

	auto player_start_data = split_string(level_components[0], ' '); 
	glm::vec3 player_start_pos = split_string_to_vec(player_start_data[1]);
	std::vector<std::string> object_components;
	// Get the individual components of the level
	for(auto i = 1; i < level_components.size() - 1; i++)
	{
		auto string_components = split_string(level_components[i], ' ');
		for(std::string str_component: string_components)
		{
			object_components.push_back(str_component);
		}
	}

	auto num_objects = 0;


	std::vector<model_t> models;
	for(auto i = 0; i < object_components.size(); i += 4)
	{
		models.push_back(model_t());
		meshes.LoadModelUnthreaded(object_components[i] + ".obj", object_components[i] + std::to_string(num_objects));
		models[i / 4].name = object_components[i] + std::to_string(num_objects);
		
		glm::vec3 scale = split_string_to_vec(object_components[i+2]);
		models[i / 4].scale = glm::vec3(50 * scale[0], 50 * scale[1], 50 * scale[2]);

		models[i / 4].pos = split_string_to_vec(object_components[i+1]); 
		models[i / 4].rotate = split_string_to_vec(object_components[i+3]);

		num_objects += 1;
	}

	std::vector<hitbox_t> hitboxes;
	for(auto& model : models)
	{
		hitboxes.push_back(model.generate_hitbox(meshes.GetVertices(model.name)));
	}
	
	glm::vec3 min = hitboxes[0].get_rotated()[0] + hitboxes[0].pos;
	glm::vec3 max = min;
	for(const auto& hitbox : hitboxes)
	{
		for(const auto& p : hitbox.get_rotated())
		{
			for(int i = 0; i < 3; i++)
			{
				min[i] = std::min(min[i], p[i] + hitbox.pos[i]);
				max[i] = std::max(max[i], p[i] + hitbox.pos[i]);
			}
		}
	}
	auto dim = max - min;
	auto center = min + dim / 2.0f;

	auto octree = create_octree(center, dim, hitboxes);


	player_t player;
	player.pos = player_start_pos;
	controls_t controls;

	if(gl.IsNewOpenGLRunning() == false)
		exit(0);

	glClearColor(0.64f, 0.72f, 0.83f, 1.0f);
	
	
	tagRECT window_map;
	GetWindowRect(window.GetHandler(), &window_map);

	auto window_center = glm::ivec2((window_map.left + window_map.right) / 2, (window_map.top + window_map.bottom) / 2);

	SetCursorPos(window_center.x, window_center.y);
	SetCursor(nullptr);

	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			for(auto& hitbox : hitboxes)
			{
				hitbox.checked = false;
			}
			check_controls(controls, window);
			update(player, octree, models, controls);
			push_updates(meshes, models, camera, player);
			draw(meshes, gl);
		}
	}
	return 0;
}

