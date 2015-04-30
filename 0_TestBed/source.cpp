#include "player.h"
#include "glm_helper.h"
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
		player.facing.accel = -controls.y * 0.0015f;
	}

	if(controls.b_jump == controls_t::ONCE && !player.in_air)
	{
		player.in_air = true;
		player.vel.y = 0.3f;
	}

	auto rot_speed_y = 0.1f;
	auto rot_speed_x = std::min(0.02f, 0.1f - glm::length(player.vel) * 0.1f); // 0.04f -> 0.02f

	auto xz_dir_old = glm::normalize(to_xz(player.facing.dir));
	
	const auto turn_angle_y = rot_speed_x * controls.delta_mouse.x * time_step; 
		//+ (controls.b_x == controls_t::ONCE ? 90.0f : 0.0f) * -controls.x; disable until can do 90.0f degrees over 1 half a second
	const auto turn_angle_x = rot_speed_y * -controls.delta_mouse.y * time_step;

	player.cumulative_turn_angle.push(turn_angle_y);

	player.facing.dir = glm::rotateY(player.facing.dir, turn_angle_y);
	player.facing.side = glm::rotateY(player.facing.side, turn_angle_y);
	player.facing.dir = glm::mat3(glm::rotate(turn_angle_x, player.facing.side)) * player.facing.dir;

	// applies friction along the ground 
	// and makes turning too fast slow you down
	/*
		xz_dir_new = axis along which the player is now facing
		xz_dir_old = axis along which the player is moving
		turn_multiplier = the amount of speed to take away based on turning too fast
		xz_vel_old = the old velocity along the xz plane
		xz_vel_new = the new velocity taking in to account friction and turn speed loss
		cumulative_turn_angle = tracker for how fast we're turning, clearing it if we hit a threshold and slow down
	*/
	if(!player.in_air)
	{
		auto xz_dir_new = glm::normalize(to_xz(player.facing.dir));
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

	if(player.vel.y <= 6 * gravity && !player.in_air)
	{
		player.in_air = true;
	}

	if(player.pos.y < 0)
	{
		player.pos.y = 0;
		player.vel.y = 0;
		player.in_air = false;
	}
}
void update(player_t& player, std::vector<model_t>& models, const controls_t& controls) 
{
	auto time_step = 1.0f;
	
	physics(player);

	update_with_controls(controls, player, time_step);


	player.update(time_step);
	

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
	camera.SetForwardVector(player.facing.dir);
	camera.CalculateView();
}

void draw(meshes_t& meshes, gl_t& gl) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	meshes.AddInstanceToRenderList("ALL");
	meshes.Render();
	
	gl.GLSwapBuffers();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	window_t window(hInstance, nCmdShow, window_callback);
	window.CreateConsoleWindow();
	window.CreateMEWindow(L"test", 1280, 720);

	auto& gl = *GLSystemSingleton::GetInstance();
	gl.InitGLDevice(window.GetHandler());
	auto& meshes = *MeshManagerSingleton::GetInstance();
	meshes.LoadModelUnthreaded("Minecraft\\MC_Creeper.obj", "Creeper");
	meshes.LoadModelUnthreaded("Building.obj", "Building");
	auto& lights = *LightManagerSingleton::GetInstance();
	lights.SetPosition(glm::vec3(0, 0, 10), 1);
	lights.SetColor(glm::vec3(1, 1, 1), 1);
	lights.SetIntensity(5.0f, 1);
	lights.SetIntensity(0.75f, 0 );
	auto& camera = *CameraSingleton::GetInstance();
	camera.SetPosition(glm::vec3());

	std::vector<model_t> models;
	models.push_back(model_t());
	models.push_back(model_t());
	models[0].name = "Creeper";
	models[0].pos.z = 10;
	models[1].name = "Building";
	models[1].pos.y = -2.602514f * 100.0f;
	models[1].scale.x = models[1].scale.z = models[1].scale.y = 100;
	player_t player;
	controls_t controls;

	if(gl.IsNewOpenGLRunning() == false)
		exit(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
			check_controls(controls, window);
			update(player, models, controls);
			push_updates(meshes, models, camera, player);
			draw(meshes, gl);
		}
	}
	return 0;
}

