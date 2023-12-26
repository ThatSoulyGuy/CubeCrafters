#include "core/Logger.hpp"
#include "core/Window.hpp"
#include "entity/Player.hpp"
#include "world/World.hpp"

Player player;

int main()
{
	Logger_Init();

	Window::Initalize();

	ShaderManager::RegisterShader(ShaderObject::Register("shaders/default", "default"));
	TextureManager::RegisterTexture(Texture::Register("textures/block.png", "block"));
	TextureManager::RegisterTexture(Texture::Register("textures/terrain.png", "atlas"));

	Window::GenerateWindow("CubeCrafters* 0.1.6", { 750, 450 });

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