#pragma once
#include "Headers.h"

namespace Truth{
	class UnityParser
	{
	private:
		std::wstring m_rootDir;


	public:
		UnityParser();
		~UnityParser();

		void Parsing(std::wstring _path);
	};
}

