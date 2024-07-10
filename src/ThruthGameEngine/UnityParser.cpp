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
/// unity 파일을 파싱해서 필요한 정보만 가져오기
/// </summary>
/// <param name="_path">유니티 프로젝트의 루트 디렉토리</param>
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
/// meta 파일만 따로 파싱하는 함수
/// </summary>
/// <param name="_path">파일 경로</param>
void Truth::UnityParser::ParseFile(fs::path& _path)
{
	// 파일이 아닌 경우 리턴
	if (!fs::is_regular_file(_path))
	{
		assert(false && "Not File Path");
		return;
	}

	// 확장자가 meta가 아닌 경우 리턴
	std::string extnName = _path.extension().generic_string();
	if (extnName != ".meta")
	{
		return;
	}

	// 파일을 열고 실패시 리턴
	std::ifstream fin(_path);
	if (!fin.is_open())
	{
		assert(false && "Cannot Open Meta File");
	}

	// 해당 파일에서 guid만 뽑아서 저장
	// 다른 정보는 아직 필요가 없다
	// 버전 관리까지 하지 않기 떄문
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
/// 디렉토리 파싱
/// </summary>
/// <param name="_path">경로</param>
void Truth::UnityParser::ParseDir(fs::path& _path)
{
	// 디렉토리가 아닌 경우 리턴 
	if (!fs::is_directory(_path))
	{
		assert(false && "Not Directory Path");
		return;
	}

	// 무시하는 디렉토리인 경우 리턴
	// unity에 있는 설정 정보는 필요가 없다.
	if (m_ignore.find(_path.filename().generic_string()) != m_ignore.end())
	{
		return;
	}

	// 해당 디렉토리에 있는 폴더와 파일
	std::queue<fs::path> dirQueue;
	std::queue<fs::path> fileQueue;

	// 해당 디렉토리에 있는 모든 파일 정보를 가져온다.
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

	// 파일과 디렉토리를 각자 파싱한다.
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
/// Yaml로 이루어진 unity 파일 파싱
/// </summary>
/// <param name="_node">Yaml Node</param>
void Truth::UnityParser::ParseYAMLFile(YAML::Node& _node, std::string& _guid)
{
	// Tag에 있는 정보를 가져와서 파싱한다.
	std::string data = _node.Tag();

	// Tag중 앞부분은 필요가 없다
	auto sData = StringConverter::split(data, ':');
	// 숫자 정보만 가져와서 각각의 ID를 가져와서 저장한다.
	auto ssData = StringConverter::split(sData.back(), '&');
	UnityNodeFormat* unf = new UnityNodeFormat{ ssData[1], ssData[0], _node };
	m_classMap[_guid][ssData[0]].push_back(unf);
	m_nodeMap[_guid][ssData[1]] = unf;
}

/// <summary>
/// Game Object tree 제거
/// </summary>
/// <param name="_node">노드</param>
void Truth::UnityParser::ResetGameObjectTree(GameObject* _node)
{
	for (auto& c : _node->m_children)
	{
		ResetGameObjectTree(c);
	}
	delete _node;
}

/// <summary>
/// 해당 prefab 노드에 Transform 정보를 역추적한다.
/// </summary>
/// <param name="_node"></param>
/// <returns></returns>
DirectX::SimpleMath::Matrix Truth::UnityParser::FindPrefabPTM(YAML::Node& _node)
{
	return Matrix::Identity;
}

/// <summary>
/// Yaml unity 파일 정리
/// Yaml-cpp 라이브러리를 사용해서 읽을 수 없는 정보를 정제하고
/// anchor에 접근이 불가능하므로 이를 보완한다.
/// 이때 원본 파일을 훼손하지 않고 정제된 버전의 파일을 복사해서 가져온다.
/// </summary>
/// <param name="_path">경로</param>
/// <returns>정제된 복사본 파일 경로</returns>
fs::path Truth::UnityParser::OrganizeUnityFile(fs::path& _path)
{
	// unity 와 prefab 파일만 읽는다 일단은
	if (_path.extension() != ".unity" && _path.extension() != ".prefab")
	{
		assert(false && "Not Unity Scene");
		return "";
	}

	// 별도로 저장할 파일 위치
	fs::path cscene = "../UnityData/" + _path.filename().generic_string();
	fs::create_directories(cscene.parent_path());

	// 파일을 열고 성공 여부를 체크
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

	// 파일 정제
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

		// stripped 단어의 경우 읽을 수 없으므로 제거한다.
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
		// fileID를 읽어 올 수 없어서 정제한다. 
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
/// Scene 파일을 파싱한다.
/// </summary>
/// <param name="_path">경로</param>
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
/// Prefab 파일을 파싱한다.
/// </summary>
/// <param name="_path">경로</param>
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
/// 파싱한 데이터를 초기화한다
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
/// Static BoxCollision에 대한 정보를 가져온다
/// </summary>
/// <returns>BoxCollison에 대한 각 꼭지점 정보</returns>
std::vector<std::vector<DirectX::SimpleMath::Vector3>> Truth::UnityParser::GetStaticMapPoint()
{
	return std::vector<std::vector<Vector3>>();
}
