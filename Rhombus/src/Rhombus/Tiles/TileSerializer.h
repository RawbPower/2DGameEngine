#pragma once

#include "Rhombus/Tiles/Tileset.h"

#include <filesystem>

namespace rhombus
{
	class TileSerializer
	{
	public:
		static bool SerializeTileset(const std::filesystem::path& filepath);
		static Ref<Tileset> DeserializeTileset(const std::string& filepath);

		static bool SerializeTileMap(const std::filesystem::path& filepath);
		static bool DeserializeTileMap(const std::filesystem::path& filepath);

	private:
		TileSerializer();
	};
}

