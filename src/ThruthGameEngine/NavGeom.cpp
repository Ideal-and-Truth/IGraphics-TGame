#include "NavGeom.h"
#include "FileUtils.h"

Truth::NavGeom::NavGeom()
	: m_bmin(std::vector<float>(3, FLT_MAX))
	, m_bmax(std::vector<float>(3, -FLT_MAX))
	, m_ver(std::vector<float>())
	, m_inx(std::vector<int32>())
{

}

Truth::NavGeom::~NavGeom()
{
	
}

void Truth::NavGeom::Load(const std::wstring& _path)
{
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	std::wstring prefix = L"../Resources/Models/";
	file->Open(prefix + _path + L".pos", FileMode::Read);

	uint32 acc = 0;
	unsigned int meshNum = file->Read<unsigned int>();
	for (unsigned int i = 0; i < meshNum; i++)
	{
		unsigned int verticesNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < verticesNum; j++)
		{
			Vector3 p;
			p.x = file->Read<float>() * 2;
			p.y = file->Read<float>() * 2;
			p.z = file->Read<float>() * 2;

			m_ver.push_back(p.x);
			m_ver.push_back(p.y);
			m_ver.push_back(p.z);

			m_bmax[0] = max(m_bmax[0], p.x);
			m_bmax[1] = max(m_bmax[1], p.y);
			m_bmax[2] = max(m_bmax[2], p.z);

			m_bmin[0] = min(m_bmin[0], p.x);
			m_bmin[1] = min(m_bmin[1], p.y);
			m_bmin[2] = min(m_bmin[2], p.z);
		}

		unsigned int indNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < indNum; j++)
		{
			m_inx.push_back(file->Read<uint32>() + acc);
		}

		acc += verticesNum;
	}
}
