#include "UnityParser.h"
#include <fstream>
#include <iostream>
#include "FileUtils.h"

Truth::UnityParser::UnityParser()
{
	m_ignore.insert(".vs");
	m_ignore.insert("Library");
	m_ignore.insert("Logs");
	m_ignore.insert("obj");
	m_ignore.insert("Packages");
	m_ignore.insert("ProjectSettings");
	m_ignore.insert("UserSettings");

	CreateBoxData();
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
/// Transformd�� ��� GameObject�� �����Ƿ� �ش� ��带 �������� GameObject�� �ľ��Ѵ�.
/// �θ𿡼� �ڽ����� �̾����� Ʈ�� ������ ��������� �ľ��Ѵ�.
/// </summary>
/// <param name="_node">�� ���</param>
/// <param name="_guid">guid</param>
/// <param name="_parent">�θ� (root�� ��� null)</param>
/// <returns>�� ��忡 �ش��ϴ� game Object ����ü</returns>
Truth::UnityParser::GameObject* Truth::UnityParser::ParseTranfomrNode(const YAML::Node& _node, const std::string& _guid, GameObject* _parent)
{
	GameObject* GO = new GameObject;
	GO->m_isCollider = false;
	GO->m_parent = _parent;
	GO->m_guid = _guid;

	// get transform data
	Vector3 pos = {
		_node["m_LocalPosition"]["x"].as<float>(),
		_node["m_LocalPosition"]["y"].as<float>(),
		_node["m_LocalPosition"]["z"].as<float>()
	};

	Quaternion rot = {
		_node["m_LocalRotation"]["x"].as<float>(),
		_node["m_LocalRotation"]["y"].as<float>(),
		_node["m_LocalRotation"]["z"].as<float>(),
		_node["m_LocalRotation"]["w"].as<float>(),
	};

	Vector3 scale = {
		_node["m_LocalScale"]["x"].as<float>(),
		_node["m_LocalScale"]["y"].as<float>(),
		_node["m_LocalScale"]["z"].as<float>()
	};

	// create Local Matrix
	Matrix LTM = Matrix::CreateScale(scale);
	LTM *= Matrix::CreateFromQuaternion(rot);
	LTM *= Matrix::CreateTranslation(pos);

	GO->m_localTM = LTM;

	// get game object
	std::string GOfid = _node["m_GameObject"]["fileID"].as<std::string>();
	GO->m_fileID = GOfid;

	const YAML::Node& GONode = m_nodeMap[_guid][GOfid]->m_node;
	if (GONode["GameObject"].IsDefined())
	{
		// get component list
		YAML::Node comList = GONode["GameObject"]["m_Component"];
		for (YAML::iterator it = comList.begin(); it != comList.end(); ++it)
		{
			// get component list
			const YAML::Node& comp = *it;
			std::string compFid = comp["component"]["fileID"].as<std::string>();

			/// find box collider
			const YAML::Node& collider = m_nodeMap[_guid][compFid]->m_node["BoxCollider"];
			if (collider.IsDefined())
			{
				GO->m_isCollider = true;
				GO->m_size.push_back({
					collider["m_Size"]["x"].as<float>(),
					collider["m_Size"]["y"].as<float>(),
					collider["m_Size"]["z"].as<float>(),
					});

				GO->m_Center.push_back({
					collider["m_Center"]["x"].as<float>(),
					collider["m_Center"]["y"].as<float>(),
					collider["m_Center"]["z"].as<float>(),
					});
			}

			/// find mesh filter
			const YAML::Node& meshFilter = m_nodeMap[_guid][compFid]->m_node["MeshFilter"];
			if (meshFilter.IsDefined())
			{
				GO->m_isMesh = true;
				m_meshFilterCount++;
				if (meshFilter["m_Mesh"]["fileID"].as<std::string>() == "10202")
				{
					GO->m_meshPath = "DebugObject/debugCube";
				}
			}
		}
	}

	// make child
	YAML::Node children = _node["m_Children"];
	for (YAML::iterator it = children.begin(); it != children.end(); ++it)
	{
		std::string childFid = (*it)["fileID"].as<std::string>();
		GO->m_children.push_back(ParseTranfomrNode(m_nodeMap[_guid][childFid]->m_node["Transform"], _guid, GO));
	}
	return GO;
}

void Truth::UnityParser::CalculateWorldTM(GameObject* _node)
{
	if (!_node->m_parent)
	{
		_node->m_worldTM = _node->m_localTM;
	}
	else
	{
		_node->m_worldTM = _node->m_localTM * _node->m_parent->m_worldTM;
	}

	for (auto* c : _node->m_children)
	{
		CalculateWorldTM(c);
	}
}

/// <summary>
/// �ڽ� �ݶ��̴��� ����� ������ �ε��� ����
/// </summary>
void Truth::UnityParser::CreateBoxData()
{
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	std::wstring path = L"../Resources/Models/DebugObject/debugCube.pos";
	file->Open(path, FileMode::Read);

	uint32 acc = 0;
	unsigned int meshNum = file->Read<unsigned int>();
	for (unsigned int i = 0; i < meshNum; i++)
	{
		unsigned int verticesNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < verticesNum; j++)
		{
			Vector3 p;
			p.x = file->Read<float>();
			p.y = file->Read<float>();
			p.z = file->Read<float>();

			m_boxVertex.push_back(p);
		}

		unsigned int indNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < indNum; j++)
		{
			m_boxindx.push_back(file->Read<uint32>() + acc);
		}
	}
}

/// <summary>
/// �Ľ��� �����͸� ���Ϸ� �ۼ��Ѵ�.
/// </summary>
void Truth::UnityParser::WriteMapData(fs::path _path)
{
	std::vector<std::vector<Vector3>> vers;
	std::vector<std::vector<uint32>> inds;

	for (auto* root : m_rootGameObject)
	{
		CalculateNodeWorldTM(root, vers, inds);
	}

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	std::wstring path = L"../Resources/MapData/";
	path += _path.replace_extension("").filename();
	path += L".map";

	file->Open(path, FileMode::Write);
	uint32 meshCount = static_cast<uint32>(vers.size());

	file->Write<uint32>(meshCount);
	for (size_t i = 0; i < meshCount; i++)
	{
		uint32 verCount = static_cast<uint32>(vers[i].size());
		file->Write<uint32>(verCount);
		for (size_t j = 0; j < verCount; j++)
		{
			file->Write<float>(vers[i][j].x);
			file->Write<float>(vers[i][j].y);
			file->Write<float>(vers[i][j].z);
		}

		uint32 indCount = static_cast<uint32>(inds[i].size());
		file->Write<uint32>(indCount);
		for (size_t j = 0; j < indCount; j++)
		{
			file->Write<uint32>(inds[i][j]);
		}
	}
}

void Truth::UnityParser::CalculateNodeWorldTM(GameObject* _node, std::vector<std::vector<Vector3>>& _vers, std::vector<std::vector<uint32>>& _inds)
{
	if (_node->m_isCollider)
	{
		for (size_t i = 0; i < _node->m_size.size(); i++)
		{
			_vers.push_back(m_boxVertex);
			_inds.push_back(m_boxindx);

			Matrix temp = Matrix::Identity;
			temp *= Matrix::CreateScale(_node->m_size[i]);
			temp *= Matrix::CreateTranslation(_node->m_Center[i]);

			temp = temp * _node->m_worldTM;
			for (auto& v : _vers.back())
			{
				v = Vector3::Transform(v, temp);
			}
		}
	}

	for (auto* c : _node->m_children)
	{
		CalculateNodeWorldTM(c, _vers, _inds);
	}
}

void Truth::UnityParser::WriteMapMeshData(fs::path _path)
{
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	std::wstring path = L"../Resources/MapData/";
	path += _path.replace_extension("").filename();
	path += L".map.mesh";

	file->Open(path, FileMode::Write);

	file->Write<uint32>(m_meshFilterCount);

	for (auto* root : m_rootGameObject)
	{
		WriteMapMeshData(root, file);
	}
}

void Truth::UnityParser::WriteMapMeshData(GameObject* _node, std::shared_ptr<FileUtils> _file)
{
	if (_node->m_isMesh)
	{
		_file->Write<std::string>(_node->m_meshPath);
		_file->Write<Matrix>(_node->m_worldTM);
	}

	for (auto* c : _node->m_children)
	{
		WriteMapMeshData(c, _file);
	}
}

/// <summary>
/// Scene ������ �Ľ��Ѵ�.
/// </summary>
/// <param name="_path">���</param>
void Truth::UnityParser::ParseSceneFile(const std::string& _path)
{
	// get scene path
	fs::path uscene(_path);
	fs::path cscene = OrganizeUnityFile(uscene);

	// copy scene data
	std::ifstream dataFile(cscene);
	std::string& guid = m_pathMap[uscene]->m_guid;

	// load yaml unity file
	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		ParseYAMLFile(doc[i], guid);
	}

	// get transform node ( 4 = transform class at unity scene file )
	for (auto& p : m_classMap[guid]["4"])
	{
		// paresing when root game object
		std::string parentFid = p->m_node["Transform"]["m_Father"]["fileID"].as<std::string>();
		if (parentFid == "0")
		{
			m_rootGameObject.push_back(ParseTranfomrNode(p->m_node["Transform"], guid, nullptr));
		}
	}

	// get prefab node ( 1001 = prefab class at unity scene file )
	for (auto& p : m_classMap[guid]["1001"])
	{
		GameObject* GO = new GameObject;
		GO->m_isCollider = false;
		GO->m_parent = nullptr;

		GO->m_guid = guid;
		GO->m_fileID = p->m_fileID;

		YAML::Node prefabNode = p->m_node["PrefabInstance"]["m_Modification"]["m_Modifications"];

		Vector3 pos = {
			prefabNode[1]["value"].as<float>(),
			prefabNode[2]["value"].as<float>(),
			prefabNode[3]["value"].as<float>()
		};
		Quaternion rot = {
			prefabNode[5]["value"].as<float>(),
			prefabNode[6]["value"].as<float>(),
			prefabNode[7]["value"].as<float>(),
			prefabNode[4]["value"].as<float>(),
		};

		GO->m_localTM = Matrix::Identity;
		GO->m_localTM *= Matrix::CreateFromQuaternion(rot);
		GO->m_localTM *= Matrix::CreateTranslation(pos);

		std::string prefabGuid = p->m_node["PrefabInstance"]["m_SourcePrefab"]["guid"].as<std::string>();

		// parse prefab file
		ParsePrefabFile(m_guidMap[prefabGuid]->m_filePath.generic_string(), GO);
		m_rootGameObject.push_back(GO);
	}

	for (auto* root : m_rootGameObject)
	{
		CalculateWorldTM(root);
	}

	WriteMapData(uscene);
	WriteMapMeshData(uscene);
}

/// <summary>
/// Prefab ������ �Ľ��Ѵ�.
/// </summary>
/// <param name="_path">���</param>
/// <param name="_parent">�θ� ������Ʈ</param>
void Truth::UnityParser::ParsePrefabFile(const std::string& _path, GameObject* _parent)
{
	// as same as scene file parse
	fs::path uscene(_path);
	fs::path cscene = OrganizeUnityFile(uscene);

	std::ifstream dataFile(cscene);

	std::string& guid = m_pathMap[uscene]->m_guid;

	auto doc = YAML::LoadAll(dataFile);
	for (size_t i = 0; i < doc.size(); i++)
	{
		ParseYAMLFile(doc[i], guid);
	}

	// get transform node ( 4 = transform class at unity scene file )
	for (auto& p : m_classMap[guid]["4"])
	{
		// paresing when root game object
		std::string parentFid = p->m_node["Transform"]["m_Father"]["fileID"].as<std::string>();
		if (parentFid == "0")
		{
			_parent->m_children.push_back(ParseTranfomrNode(p->m_node["Transform"], guid, _parent));
		}
	}

	// get prefab node ( 1001 = prefab class at unity scene file )
	for (auto& p : m_classMap[guid]["1001"])
	{
		GameObject* GO = new GameObject;
		GO->m_isCollider = false;
		YAML::Node prefabNode = p->m_node["PrefabInstance"]["m_Modification"]["m_Modifications"];

		Vector3 pos = {
			prefabNode[1]["value"].as<float>(),
			prefabNode[2]["value"].as<float>(),
			prefabNode[3]["value"].as<float>()
		};
		Quaternion rot = {
			prefabNode[5]["value"].as<float>(),
			prefabNode[6]["value"].as<float>(),
			prefabNode[7]["value"].as<float>(),
			prefabNode[4]["value"].as<float>(),
		};

		GO->m_localTM = Matrix::Identity;
		GO->m_localTM *= Matrix::CreateFromQuaternion(rot);
		GO->m_localTM *= Matrix::CreateTranslation(pos);

		std::string prefabGuid = p->m_node["PrefabInstance"]["m_SourcePrefab"]["guid"].as<std::string>();

		// parse prefab file
		ParsePrefabFile(m_guidMap[prefabGuid]->m_filePath.generic_string(), GO);
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

	for (auto& r : m_rootGameObject)
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
