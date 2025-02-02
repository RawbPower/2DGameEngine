#include "rbpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace rhombus {

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		Log::Assert(false, "RendererAPI::None is currently not supported"); return nullptr;

			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(filepath);
		}

		Log::Assert(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader>  Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& pixelSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		Log::Assert(false, "RendererAPI::None is currently not supported"); return nullptr;

			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(name, vertexSrc, pixelSrc);
		}

		Log::Assert(false, "Unknown RendererAPI");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		Log::Assert(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	rhombus::Ref<Shader> ShaderLibrary::Load(const std::string& filepath) 
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	rhombus::Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) 
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	rhombus::Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		Log::Assert(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}