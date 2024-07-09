#include "UnityParser.h"
#include <fstream>

Truth::UnityParser::UnityParser()
{
	m_ignore.insert(".vs");
	m_ignore.insert("Library");
	m_ignore.insert("Logs");
	m_ignore.insert("obj");
	m_ignore.insert("Packages");
	m_ignore.insert("ProjectSettings");
	m_ignore.insert("UserSettings");
}

Truth::UnityParser::~UnityParser()
{

}

/// <summary>
/// unity 파일을 파싱해서 필요한 정보만 가져오기
/// </summary>
/// <param name="_path">유니티 프로젝트의 루트 디렉토리</param>
void Truth::UnityParser::Parsing(const std::string& _path)
{
	fs::path root(_path);

	if (!fs::exists(root))
	{
		assert(false && "Wrong File or Directory Path");
		return;
	}
	if (fs::is_directory(root))
	{
		ParsingDir(root);
	}
	else if (fs::is_regular_file(root))
	{
		ParsingFile(root);
	}
}

void Truth::UnityParser::ParsingFile(fs::path& _path)
{
	if (!fs::is_regular_file(_path))
	{
		assert(false && "Not File Path");
		return;
	}

	std::string extnName = _path.extension().generic_string();
	if (extnName != ".meta")
	{
		return;
	}

	std::ifstream fin(_path);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Meta File");
	}

	std::string line;
	while (fin)
	{
		getline(fin, line);

		StringConverter::DeleteAlpha(line, ' ');
		std::vector<std::string> sLine = StringConverter::split(line, ':');
		if (!sLine.empty() && sLine[0] == "guid")
		{
			UnityFormat uf
			{
				sLine[1],
				_path,
				_path.replace_extension()
			};

			m_fileMap[uf.m_guid] = uf;

			break;
		}
	}
}

void Truth::UnityParser::ParsingDir(fs::path& _path)
{
	if (!fs::is_directory(_path))
	{
		assert(false && "Not Directory Path");
		return;
	}

	if (m_ignore.find(_path.filename().generic_string()) != m_ignore.end())
	{
		return;
	}

	std::queue<fs::path> dirQueue;
	std::queue<fs::path> fileQueue;

	fs::directory_iterator itr(_path);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			dirQueue.push(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			fileQueue.push(childPath);
		}
		itr++;
	}

	while (!fileQueue.empty())
	{
		ParsingFile(fileQueue.front());
		fileQueue.pop();
	}

	while (!dirQueue.empty())
	{
		ParsingDir(dirQueue.front());
		dirQueue.pop();
	}
}

bool Truth::UnityParser::IsClass(const std::string& _name, UNITY_CLASS_ID _id)
{
	return _name == m_classPrefix + std::to_string(static_cast<int32>(_id));
}

void Truth::UnityParser::ParsingYAMLFile(YAML::Node _node)
{
	for (YAML::iterator iter = _node.begin(); iter != _node.end(); ++iter)
	{
		DEBUG_PRINT((iter->first.as<std::string>() + " : ").c_str());
		if (iter->second.IsMap())
		{
			DEBUG_PRINT("\n");
			ParsingYAMLFile(iter->second);
		}
		else if (iter->second.IsScalar())
		{
			DEBUG_PRINT((iter->second.as<std::string>()).c_str());
		}
		else if (iter->second.IsSequence())
		{
			for (std::size_t idx = 0; idx < iter->second.size(); idx++)
			{
				if (iter->second[idx].IsMap())
				{
					DEBUG_PRINT("\n");
					ParsingYAMLFile(iter->second[idx]);
				}
				else
				{
					DEBUG_PRINT((iter->second[idx].as<std::string>() + ", ").c_str());
				}
			}
		}
		DEBUG_PRINT("\n");
	}
}

void Truth::UnityParser::CreateMapCollision(const std::string& _path)
{
	fs::path uscene(_path);

	if (uscene.extension() != ".unity")
	{
		assert(false && "Not Unity Scene");
		return;
	}

	fs::path cscene = "../UnityData/" + uscene.filename().generic_string();
	fs::create_directories(cscene.parent_path());

	std::vector<std::vector<Vector3>> m_points;

	std::ifstream fin(uscene);
	std::ofstream fout(cscene);

	if (!fin.is_open())
	{
		assert(false && "Cannot Open Scene File");
	}
	if (!fout.is_open())
	{
		assert(false && "Cannot Open Duplicate Scene File");
	}

	std::string line;
	bool isStripped = false;
	while (fin)
	{
		getline(fin, line);
		if (isStripped)
		{
			line = "S" + line;
			isStripped = false;
		}
		auto sLine = StringConverter::split(line, ' ');
		if (!sLine.empty() && sLine.back() == "stripped")
		{
			sLine.pop_back();
			line = "";
			for (auto& s : sLine)
			{
				line += s + " ";
			}
			line.pop_back();
			isStripped = true;
		}

		fout << line;
		fout << "\n";
	}

	fin.close();
	fout.close();

	std::ifstream dataFile(cscene);

	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		ParsingYAMLFile(doc[i]);
	}
}
