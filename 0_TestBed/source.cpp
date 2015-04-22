#include <MyEngine.h>
#include <SFML\Graphics.hpp>
#include <memory>
LRESULT CALLBACK window_callback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Callbacks for winapp 
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

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	auto test = std::unique_ptr<int>(new int(2));

	WindowClass window(hInstance, nCmdShow, window_callback);

	window.CreateMEWindow(L"test", 640, 480);

	auto& gl = *GLSystemSingleton::GetInstance();

	gl.InitGLDevice(window.GetHandler());

	
	auto& meshes = *MeshManagerSingleton::GetInstance();
	auto& lights = *LightManagerSingleton::GetInstance();
	
	lights.SetPosition( glm::vec3(0, 0, 10), 1);
	lights.SetColor( glm::vec3(1, 1, 1), 1);
	lights.SetIntensity (5.0f, 1);
	lights.SetIntensity (0.75f, 0 );

	meshes.LoadModelUnthreaded("Minecraft\\MC_Creeper.obj", "Creeper");

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
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			meshes.AddInstanceToRenderList("ALL");
			meshes.Render();
	
			gl.GLSwapBuffers();
		}
	}
	return 0;
}