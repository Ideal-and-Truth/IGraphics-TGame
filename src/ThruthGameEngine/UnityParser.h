#pragma once
#include "Headers.h"
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

namespace Truth {
	/// <summary>
	/// �� Ŭ������ Debug ��忡���� �۵��ϵ��� ���� ����
	/// ���� �����Ϳ����� �۵��ϴ� Ŭ����
	/// </summary>
	class UnityParser
	{
	private:
		/// <summary>
		/// Node ������ �����ϴ� ����ü
		/// </summary>
		struct UnityNodeFormat
		{
			std::string m_fileID;
			std::string m_classID;
			YAML::Node m_node;
		};

		/// <summary>
		/// guid�� ���� ������ �����ϴ� ����ü
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
			std::string m_guid;
			std::string m_fileID;
			std::string m_classID;

			Matrix m_localTM;

			GameObject* m_parent;
			std::vector<GameObject*> m_children;
		};

		std::vector<GameObject*> m_gameObject;

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
		void ParsePrefabFile(const std::string& _path);

		void Reset();

		std::vector<std::vector<Vector3>> GetStaticMapPoint();

	private:
		void ParseFile(fs::path& _path);
		void ParseDir(fs::path& _path);

		void ParseYAMLFile(YAML::Node& _node, std::string& _guid);

		void ResetGameObjectTree(GameObject* _node);

		Matrix FindPrefabPTM(YAML::Node& _node);

		fs::path OrganizeUnityFile(fs::path& _path);
	};
}

