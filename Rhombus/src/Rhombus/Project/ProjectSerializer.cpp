#include "rbpch.h"
#include "ProjectSerializer.h"

#include <fstream>

#define YAML_CPP_STATIC_DEFINE		// Needed for yaml static library to work for some reason
#include <yaml-cpp/yaml.h>

namespace rhombus
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				out << YAML::Key << "ScriptDirectory" << YAML::Value << config.ScriptDirectory.string();

				out << YAML::Key << "GameWidth" << YAML::Value << config.GameWidth;
				out << YAML::Key << "GameHeight" << YAML::Value << config.GameHeight;
				out << YAML::Key << "WindowWidth" << YAML::Value << config.WindowWidth;
				out << YAML::Key << "WindowHeight" << YAML::Value << config.WindowHeight;
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			Log::Error("Failed to load project file '%s'\n     {1}", filepath, e.what());
			return false;
		}

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.ScriptDirectory = projectNode["ScriptDirectory"].as<std::string>();

		config.GameWidth = projectNode["GameWidth"].as<uint32_t>();
		config.GameHeight = projectNode["GameHeight"].as<uint32_t>();
		config.WindowWidth = projectNode["WindowWidth"].as<uint32_t>();
		config.WindowHeight = projectNode["WindowHeight"].as<uint32_t>();
		return true;
	}
}