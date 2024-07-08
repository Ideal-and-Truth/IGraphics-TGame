#pragma once
#include "Headers.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Truth {
	class UnityParser
	{

	private:
		struct UnityFormat
		{
			uint64 m_guid;
			fs::path m_metaPath;
			fs::path m_filePath;
		};

	public:
		UnityParser();
		~UnityParser();

		void Parsing(std::string _path);

	private:
		void ParsingFile(fs::path& _path);
		void ParsingDir(fs::path& _path);

		void IntegrateUnityFile();
	};
}

