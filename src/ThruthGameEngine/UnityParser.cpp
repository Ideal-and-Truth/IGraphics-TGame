#include "UnityParser.h"
#include <fstream>
#include <iostream>

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
	Reset();
}

/// <summary>
/// unity ������ �Ľ��ؼ� �ʿ��� ������ ��������
/// </summary>
/// <param name="_path">����Ƽ ������Ʈ�� ��Ʈ ���丮</param>
void Truth::UnityParser::SetRootDir(const std::string& _path)
{
	fs::path root(_path);

	if (!fs::exists(root))
	{
		assert(false && "Wrong File or Directory Path");
		return;
	}
	if (fs::is_directory(root))
	{
		ParseDir(root);
	}
	else if (fs::is_regular_file(root))
	{
		ParseFile(root);
	}
}

/// <summary>
/// meta ���ϸ� ���� �Ľ��ϴ� �Լ�
/// </summary>
/// <param name="_path">���� ���</param>
void Truth::UnityParser::ParseFile(fs::path& _path)
{
	// ������ �ƴ� ��� ����
	if (!fs::is_regular_file(_path))
	{
		assert(false && "Not File Path");
		return;
	}

	// Ȯ���ڰ� meta�� �ƴ� ��� ����
	std::string extnName = _path.extension().generic_string();
	if (extnName != ".meta")
	{
		return;
	}

	// ������ ���� ���н� ����
	std::ifstream fin(_path);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Meta File");
	}

	// �ش� ���Ͽ��� guid�� �̾Ƽ� ����
	// �ٸ� ������ ���� �ʿ䰡 ����
	// ���� �������� ���� �ʱ� ����
	std::string line;
	while (fin)
	{
		getline(fin, line);

		StringConverter::DeleteAlpha(line, ' ');
		std::vector<std::string> sLine = StringConverter::split(line, ':');
		if (!sLine.empty() && sLine[0] == "guid")
		{
			UnityFileFormat* uf = new UnityFileFormat{
				sLine[1],
				_path,
				_path.replace_extension()
			};


			m_guidMap[uf->m_guid] = uf;
			m_pathMap[uf->m_filePath] = uf;
			break;
		}
	}
}

/// <summary>
/// ���丮 �Ľ�
/// </summary>
/// <param name="_path">���</param>
void Truth::UnityParser::ParseDir(fs::path& _path)
{
	// ���丮�� �ƴ� ��� ���� 
	if (!fs::is_directory(_path))
	{
		assert(false && "Not Directory Path");
		return;
	}

	// �����ϴ� ���丮�� ��� ����
	// unity�� �ִ� ���� ������ �ʿ䰡 ����.
	if (m_ignore.find(_path.filename().generic_string()) != m_ignore.end())
	{
		return;
	}

	// �ش� ���丮�� �ִ� ������ ����
	std::queue<fs::path> dirQueue;
	std::queue<fs::path> fileQueue;

	// �ش� ���丮�� �ִ� ��� ���� ������ �����´�.
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

	// ���ϰ� ���丮�� ���� �Ľ��Ѵ�.
	while (!fileQueue.empty())
	{
		ParseFile(fileQueue.front());
		fileQueue.pop();
	}

	while (!dirQueue.empty())
	{
		ParseDir(dirQueue.front());
		dirQueue.pop();
	}
}

/// <summary>
/// Yaml�� �̷���� unity ���� �Ľ�
/// </summary>
/// <param name="_node">Yaml Node</param>
void Truth::UnityParser::ParseYAMLFile(YAML::Node& _node, std::string& _guid)
{
	// Tag�� �ִ� ������ �����ͼ� �Ľ��Ѵ�.
	std::string data = _node.Tag();

	// Tag�� �պκ��� �ʿ䰡 ����
	auto sData = StringConverter::split(data, ':');
	// ���� ������ �����ͼ� ������ ID�� �����ͼ� �����Ѵ�.
	auto ssData = StringConverter::split(sData.back(), '&');
	UnityNodeFormat* unf = new UnityNodeFormat{ ssData[1], ssData[0], _node };
	m_classMap[_guid][ssData[0]].push_back(unf);
	m_nodeMap[_guid][ssData[1]] = unf;
}

/// <summary>
/// Game Object tree ����
/// </summary>
/// <param name="_node">���</param>
void Truth::UnityParser::ResetGameObjectTree(GameObject* _node)
{
	for (auto& c : _node->m_children)
	{
		ResetGameObjectTree(c);
	}
	delete _node;
}

/// <summary>
/// �ش� prefab ��忡 Transform ������ �������Ѵ�.
/// </summary>
/// <param name="_node"></param>
/// <returns></returns>
DirectX::SimpleMath::Matrix Truth::UnityParser::FindPrefabPTM(YAML::Node& _node)
{
	return Matrix::Identity;
}

/// <summary>
/// Yaml unity ���� ����
/// Yaml-cpp ���̺귯���� ����ؼ� ���� �� ���� ������ �����ϰ�
/// anchor�� ������ �Ұ����ϹǷ� �̸� �����Ѵ�.
/// �̶� ���� ������ �Ѽ����� �ʰ� ������ ������ ������ �����ؼ� �����´�.
/// </summary>
/// <param name="_path">���</param>
/// <returns>������ ���纻 ���� ���</returns>
fs::path Truth::UnityParser::OrganizeUnityFile(fs::path& _path)
{
	// unity �� prefab ���ϸ� �д´� �ϴ���
	if (_path.extension() != ".unity" && _path.extension() != ".prefab")
	{
		assert(false && "Not Unity Scene");
		return "";
	}

	// ������ ������ ���� ��ġ
	fs::path cscene = "../UnityData/" + _path.filename().generic_string();
	fs::create_directories(cscene.parent_path());

	// ������ ���� ���� ���θ� üũ
	std::ifstream fin(_path);
	std::ofstream fout(cscene);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Scene File");
	}
	if (!fout.is_open())
	{
		assert(false && "Cannot Open Duplicate Scene File");
	}

	// ���� ����
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

		// stripped �ܾ��� ��� ���� �� �����Ƿ� �����Ѵ�.
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
		// fileID�� �о� �� �� ��� �����Ѵ�. 
		if (!sLine.empty() && sLine[0] == "---")
		{
			line = sLine[0] + " " + sLine[1] + sLine[2];
		}

		fout << line;
		fout << "\n";
	}

	fin.close();
	fout.close();

	return cscene;
}

/// <summary>
/// Scene ������ �Ľ��Ѵ�.
/// </summary>
/// <param name="_path">���</param>
void Truth::UnityParser::ParseSceneFile(const std::string& _path)
{
	fs::path uscene(_path);
	fs::path cscene = OrganizeUnityFile(uscene);
	
	std::ifstream dataFile(cscene);

	std::string& guid = m_pathMap[uscene]->m_guid;

	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		ParseYAMLFile(doc[i], guid);
	}

	for (auto& p : m_classMap[guid]["1001"])
	{
		const YAML::Node& rootNode = p->m_node["PrefabInstance"]["m_Modification"];
		const YAML::Node& targetNode = rootNode["m_Modifications"];

		auto target = p->m_node["m_Modifications"];
	}
}

/// <summary>
/// Prefab ������ �Ľ��Ѵ�.
/// </summary>
/// <param name="_path">���</param>
void Truth::UnityParser::ParsePrefabFile(const std::string& _path)
{
	fs::path uscene(_path);
	fs::path cscene = OrganizeUnityFile(uscene);

	std::ifstream dataFile(cscene);

	std::string& guid = m_pathMap[uscene]->m_guid;

	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		ParseYAMLFile(doc[i], guid);
	}
}

/// <summary>
/// �Ľ��� �����͸� �ʱ�ȭ�Ѵ�
/// </summary>
void Truth::UnityParser::Reset()
{
	for (auto& fn : m_nodeMap)
	{
		for (auto& n : fn.second)
		{
			delete n.second;
		}
	}

	for (auto& fn : m_guidMap)
	{
		delete fn.second;
	}
	
	for (auto& r : m_gameObject)
	{
		ResetGameObjectTree(r);
	}

	m_guidMap.clear();
	m_pathMap.clear();
	m_nodeMap.clear();
	m_classMap.clear();
}

/// <summary>
/// Static BoxCollision�� ���� ������ �����´�
/// </summary>
/// <returns>BoxCollison�� ���� �� ������ ����</returns>
std::vector<std::vector<DirectX::SimpleMath::Vector3>> Truth::UnityParser::GetStaticMapPoint()
{
	return std::vector<std::vector<Vector3>>();
}
