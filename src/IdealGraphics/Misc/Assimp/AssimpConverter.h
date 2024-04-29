#pragma once
#include "Core/Core.h"

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include "Misc/Assimp/AssimpConvertType.h"

namespace AssimpConvert
{
	class Mesh;
	class Material;
	class Bone;
}

class AssimpConverter
{
public:
	AssimpConverter();
	~AssimpConverter();

public:
	static std::wstring ConvertStringToWString(const std::string& str);
	static std::string ConvertWStringToString(const std::wstring& wstr);
	static void Replace(std::string& OutStr, std::string Comp, std::string Rep);
	static void Replace(std::wstring& OutStr, std::wstring Comp, std::wstring Rep);

public:
	void SetAssetPath(const std::wstring& AssetPath) { m_assetPath = AssetPath; }
	void SetModelPath(const std::wstring& ModelPath) { m_modelPath = ModelPath; }
	void SetTexturePath(const std::wstring& TexturePath) { m_texturePath = TexturePath; }

public:
	void ReadAssetFile(const std::wstring& path);
	void ExportModelData(std::wstring savePath, bool IsSkinnedData = false);
	void ExportMaterialData(const std::wstring& savePath);
	void ExportAnimationData(std::wstring savePath, uint32 index = 0);

private:
	std::string WriteTexture(std::string SaveFolder, std::string File);
	void WriteMaterialData(std::wstring FilePath);
	void WriteModelFile(const std::wstring& filePath);
	void WriteSkinnedModelFile(const std::wstring& filePath);
	void ReadModelData(aiNode* node, int32 index, int32 parent);
	void ReadSkinnedModelData(aiNode* node, int32 index, int32 parent);
	void ReadMaterialData();
	void ReadSkinData();
	void ReadMeshData(aiNode* node, int32 bone);
	void ReadSkinnedMeshData(aiNode* node, int32 bone);

	std::shared_ptr<AssimpConvert::Animation> ReadAnimationData(aiAnimation* srcAnimation);
	std::shared_ptr<AssimpConvert::AnimationNode> ParseAnimationNode(std::shared_ptr<AssimpConvert::Animation> animation, aiNodeAnim* srcNode);
	void ReadKeyFrameData(std::shared_ptr<AssimpConvert::Animation> animation, aiNode* node, std::map<std::string, std::shared_ptr<AssimpConvert::AnimationNode>>& cache);
	void WriteAnimationData(std::shared_ptr<AssimpConvert::Animation> animation, std::wstring finalPath);
	uint32 GetBoneIndex(const std::string& name);

private:
	
	std::shared_ptr<Assimp::Importer> m_importer;
	const aiScene* m_scene;

	std::wstring m_assetPath = L"Resources/Assets/";
	std::wstring m_modelPath = L"Resources/Models/";
	std::wstring m_texturePath = L"Resources/Textures/";

private:
	bool m_isSkinnedData = false;

	std::vector<std::shared_ptr<AssimpConvert::Material>> m_materials;
	std::vector<std::shared_ptr<AssimpConvert::Mesh>> m_meshes;
	std::vector<std::shared_ptr<AssimpConvert::SkinnedMesh>> m_skinnedMeshes;
	std::vector<std::shared_ptr<AssimpConvert::Bone>> m_bones;
};