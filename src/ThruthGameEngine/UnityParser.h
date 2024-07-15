#pragma once
#include "Headers.h"
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace Truth
{
	/// <summary>
	/// 이 클래스는 Debug 모드에서만 작동하도록 만들 예정
	/// 오직 에디터에서만 작동하는 클래스
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
		/// </summary>
		struct GameObject
		{
			bool isCollider = false;
			Vector3 m_size = { 1.0f, 1.0f, 1.0f };
			Vector3 m_Center = { 0.0f, 0.0f, 0.0f };

			std::string m_guid = "";
			std::string m_fileID = "";

			Matrix m_localTM = Matrix::Identity;
			Matrix m_worldTM = Matrix::Identity;

			GameObject* m_parent = nullptr;
			std::vector<GameObject*> m_children;
		};

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

	public:
		UnityParser();
		~UnityParser();

		void SetRootDir(const std::string& _path);
		void ParseSceneFile(const std::string& _path);
		void ParsePrefabFile(const std::string& _path, GameObject* _parent);

		void Reset();

		std::vector<std::vector<Vector3>> GetStaticMapPoint();

	private:
		void ParseFile(fs::path& _path);
		void ParseDir(fs::path& _path);

		void ParseYAMLFile(YAML::Node& _node, std::string& _guid);

		void ResetGameObjectTree(GameObject* _node);

		fs::path OrganizeUnityFile(fs::path& _path);

		GameObject* ParseTranfomrNode(const YAML::Node& _node, const std::string& _guid, GameObject* _parent);

		void CalculateWorldTM(GameObject* _node);

		void CreateBoxData();
		void WriteMapData(fs::path _path);
		void SetMapData(GameObject* _node, std::vector<std::vector<Vector3>>& _vers, std::vector<std::vector<uint32>>& _inds);
	};
}

