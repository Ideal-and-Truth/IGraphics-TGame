#pragma once
#include "Headers.h"
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace Truth {
	class UnityParser
	{
	private:
		enum class UNITY_CLASS_ID
		{
			GameObject = 1,
		};

		struct UnityFormat
		{
			std::string m_guid;
			fs::path m_metaPath;
			fs::path m_filePath;
		};

		std::set<std::string> m_ignore;

		std::map<std::string, UnityFormat> m_fileMap;

		const std::string m_classPrefix = "!u!";


	public:
		UnityParser();
		~UnityParser();

		void Parsing(const std::string& _path);
		void CreateMapCollision(const std::string& _path);

	private:
		void ParsingFile(fs::path& _path);
		void ParsingDir(fs::path& _path);

		bool IsClass(const std::string& _name, UNITY_CLASS_ID _id);

		void ParsingYAMLFile(YAML::Node _node);
	};
}

