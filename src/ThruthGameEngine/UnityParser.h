#pragma once
#include "Headers.h"
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

class FileUtils;

namespace Truth
{
	/// <summary>
	/// 이 클래스는 Debug 모드에서만 작동하도록 만들 예정
	/// 오직 에디터에서만 작동하는 클래스
	/// 해당 클래스로 얻은 정보는 에디터상에서 편집 불가능
	/// </summary>
	class UnityParser
	{
	private:
		/// <summary>
		/// Node 정보를 저장하는 구조체
		/// </summary>
		struct UnityNodeFormat
		{
			std::string m_fileID;
			std::string m_classID;
			YAML::Node m_node;
		};

		/// <summary>
		/// guid와 파일 정보를 저장하는 구조체
		/// </summary>
		struct UnityFileFormat
		{
			std::string m_guid;
			fs::path m_metaPath;
			fs::path m_filePath;
		};

		/// <summary>
		/// unity game object tree
		/// 이를 기반으로 Entity Tree를 재구성
		/// </summary>
		struct GameObject
		{
			// Collider Info
			bool m_isCollider = false;
			std::vector<Vector3> m_size;
			std::vector<Vector3> m_Center;
			
			// Mesh Filter Info
			bool m_isMesh = false;
			std::string m_meshPath = "";

			// Transform Info
			Matrix m_localTM = Matrix::Identity;
			Matrix m_worldTM = Matrix::Identity;

			std::string m_guid = "";
			std::string m_fileID = "";

			GameObject* m_parent = nullptr;
			std::vector<GameObject*> m_children;
		};

		uint32 m_meshFilterCount = 0;

		std::vector<GameObject*> m_rootGameObject;

		std::vector<Vector3> m_boxVertex;
		std::vector<uint32> m_boxindx;

		std::set<std::string> m_ignore;

		// guid, data
		std::map<std::string, UnityFileFormat*> m_guidMap;
		// path, data
		std::map<fs::path, UnityFileFormat*> m_pathMap;

		// guid, fileID, node
		std::map<std::string, std::map<std::string, UnityNodeFormat*>> m_nodeMap;
		// guid, classID, vector<node>
		std::map<std::string, std::map<std::string, std::vector<UnityNodeFormat*>>> m_classMap;

		const fs::path m_defaultPath = "../Resources/MapData/";
		const fs::path m_debugCubePath = "DebugObject/debugCube";
		fs::path m_sceneName;
		
	public:
		UnityParser();
		~UnityParser();

		void SetRootDir(const std::string& _path);
		void ParseSceneFile(const std::string& _path);

		void ParseUnityFile(const std::string& _path);

		void Reset();

		std::vector<std::vector<Vector3>> GetStaticMapPoint();

	private:
		void ParseFile(fs::path& _path);
		void ParseDir(fs::path& _path);

		void ParseYAMLFile(YAML::Node& _node, const std::string& _guid);

		void ResetGameObjectTree(GameObject* _node);

		fs::path OrganizeUnityFile(fs::path& _path);

		GameObject* ParseTranfomrNode(const YAML::Node& _node, const std::string& _guid, GameObject* _parent);
		GameObject* ParsePrefabNode(const YAML::Node& _node, const std::string& _guid, GameObject* _parent);
		
		void ReadPrefabFile(const fs::path& _path, GameObject* _parent);

		void ParseGameObject(const std::string& _guid, const YAML::Node& _node, GameObject* _owner);
		void ParseBoxCollider(const YAML::Node& _node, GameObject* _owner);
		void ParseMeshFilter(const YAML::Node& _node, GameObject* _owner);

		Matrix GetPrefabMatrix(const YAML::Node& _node);

		void CalculateWorldTM(GameObject* _node);
		void CreateBoxData();

		void WriteMapData();
		void GetColliderVertexes(GameObject* _node, std::vector<std::vector<Vector3>>& _vers, std::vector<std::vector<uint32>>& _inds);

		void WriteMapMeshData(fs::path _path);
		void WriteMapMeshData(GameObject* _node, std::shared_ptr<FileUtils> _file);

		std::string ConvertUnloadedMesh(fs::path _path);
	};
}

