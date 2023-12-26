#include <cassert>

#include "core/Logger.hpp"
#include "core/Window.hpp"
#include "entity/Player.hpp"
#include "world/World.hpp"

Player player;

int main(int argc, char* argv[])
{
	Logger_Init();

	Window::Initalize();

	ShaderManager::RegisterShader(ShaderObject::Register("shaders/default", "default"));
	ShaderManager::RegisterShader(ShaderObject::Register("shaders/wireframe", "wireframe"));
	TextureManager::RegisterTexture(Texture::Register("textures/block.png", "block"));
	TextureManager::RegisterTexture(Texture::Register("textures/terrain.png", "atlas"));

	int scr_width{750};
	int scr_height{450};
	if (argc > 1)
	{
		assert(argc == 3);
		scr_width = std::stoi(argv[1]);
		scr_height = std::stoi(argv[2]);
	}
	Window::GenerateWindow("CubeCrafters* 0.1.8", { scr_width, scr_height });

	Input::Initialize(Window::window);

	player.Initialize({ 0, 20, 0 });

	Logger_WriteConsole("Hello, CubeCrafters!", LogLevel::INFO);

	World::StartUpdating();

	while (!Window::ShouldClose())
	{
		Window::UpdateColors();

		Input::UpdateInput();

		player.Update();
		
		MainThreadExecutor::UpdateTasks();
		World::playerPosition = player.transform.position;

		Renderer::RenderObjects(player.camera);

		Window::UpdateBuffers();
	}

	World::StopUpdating();

	ShaderManager::CleanUp();
	Renderer::CleanUp();
	Window::CleanUp();
	Logger_CleanUp();

	return 0;
}