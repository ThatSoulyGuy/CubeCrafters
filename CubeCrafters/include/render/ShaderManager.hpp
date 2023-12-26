#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <map>
#include <memory>
#include "render/ShaderObject.hpp"

namespace ShaderManager
{
	std::map<std::string, std::shared_ptr<ShaderObject>> registeredShaders;

	void RegisterShader(std::unique_ptr<ShaderObject> shader)
	{
		Logger_FunctionStart;

		Logger_WriteConsole("Registered Shader Object: '" + shader->name + "'", LogLevel::INFO);

		registeredShaders.insert({ shader->name, std::move(shader) });

		Logger_FunctionEnd;
	}

	std::shared_ptr<ShaderObject> GetShader(const std::string& name)
	{
		auto it = registeredShaders.find(name);

		if (it != registeredShaders.end()) 
			return std::make_shared<ShaderObject>(*(it->second));
		else 
			return nullptr;
	}

	void CleanUp()
	{
		for (auto& [key, shader] : registeredShaders)
			shader->CleanUp();
	}
}

#endif // !SHADER_MANAGER_HPP