#pragma once
#include "Headers.h"

namespace Truth
{
	class NavGeom
	{
	public:
		std::vector<float> m_ver;
		std::vector<int32> m_inx;

		std::vector<float> m_bmin;
		std::vector<float> m_bmax;


	public:
		NavGeom();
		~NavGeom();

		void Load(const std::wstring& _path);
	};
}

