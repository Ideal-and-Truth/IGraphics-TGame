#include "Misc/Assimp/AssimpConverter.h"

#include "Misc/Utils/tinyxml2.h"
#include "Misc/Utils/FileUtils.h"
#include <filesystem>

AssimpConverter::AssimpConverter()
	: m_scene(nullptr)
{
	m_importer = std::make_shared<Assimp::Importer>();
}

AssimpConverter::~AssimpConverter()
{

}

std::wstring AssimpConverter::ConvertStringToWString(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::string AssimpConverter::ConvertWStringToString(const std::wstring& wstr)
{
	return std::string(wstr.begin(), wstr.end());
}

void AssimpConverter::Replace(std::string& OutStr, std::string Comp, std::string Rep)
{
	std::string temp = OutStr;
	size_t start_pos = 0;
	while ((start_pos = temp.find(Comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, Comp.length(), Rep);
		start_pos += Rep.length();
	}
	OutStr = temp;
}

void AssimpConverter::Replace(std::wstring& OutStr, std::wstring Comp, std::wstring Rep)
{
	std::wstring temp = OutStr;
	size_t start_pos = 0;
	while ((start_pos = temp.find(Comp, start_pos)) != std::wstring::npos)
	{
		temp.replace(start_pos, Comp.length(), Rep);
		start_pos += Rep.length();
	}
	OutStr = temp;
}

void AssimpConverter::ReadAssetFile(const std::wstring& path)
{
	std::wstring fileStr;
	fileStr = m_assetPath + path;

	uint32 flag = 0;
	flag |= aiProcess_ConvertToLeftHanded;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_GenNormals;
	flag |= aiProcess_CalcTangentSpace;
	//flag |= aiProcess_OptimizeMeshes;
	//flag |= aiProcess_PreTransformVertices;
	m_importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

	m_scene = m_importer->ReadFile(
		ConvertWStringToString(fileStr),
		flag
	);

	if (!m_scene)
	{
		printf(m_importer->GetErrorString());
		assert(false);
		MessageBox(NULL, path.c_str(), L"AssimpConverterReadSsetFile", MB_OK);
	}

}

void AssimpConverter::ExportModelData(std::wstring savePath, bool IsSkinnedData /*= false*/)
{
	// skinned data일 경우 
	m_isSkinnedData = IsSkinnedData;

	if (m_isSkinnedData)
	{
		std::wstring finalPath = m_modelPath + savePath + L".dmesh";
		ReadSkinnedModelData(m_scene->mRootNode, -1, -1);
		ReadSkinData();
		WriteSkinnedModelFile(finalPath);
		// todo : write

		//Write CSV File
		{
			FILE* file;
			fopen_s(&file, "../Vertices.csv", "w");

			for (std::shared_ptr<AssimpConvert::Bone>& bone : m_bones)
			{
				std::string name = bone->name;
				::fprintf(file, "%d,%s\n", bone->index, bone->name.c_str());
			}

			fprintf(file, "\n");

			for (std::shared_ptr<AssimpConvert::SkinnedMesh>& mesh : m_skinnedMeshes)
			{
				std::string name = mesh->name;
				::printf("%s\n", name.c_str());

				for (UINT i = 0; i < mesh->vertices.size(); i++)
				{
					Vector3 p = mesh->vertices[i].Position;
					Vector4 indices = mesh->vertices[i].BlendIndices;
					Vector4 weights = mesh->vertices[i].BlendWeights;

					::fprintf(file, "%f,%f,%f,", p.x, p.y, p.z);
					::fprintf(file, "%f,%f,%f,%f,", indices.x, indices.y, indices.z, indices.w);
					::fprintf(file, "%f,%f,%f,%f\n", weights.x, weights.y, weights.z, weights.w);
				}
			}

			fclose(file);
		}
	}
	else
	{
		std::wstring finalPath = m_modelPath + savePath + L".mesh";
		ReadModelData(m_scene->mRootNode, -1, -1);
		WriteModelFile(finalPath);
	}


}

void AssimpConverter::ExportMaterialData(const std::wstring& savePath)
{
	std::wstring filePath = m_texturePath + savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(filePath);
}

void AssimpConverter::ExportAnimationData(std::wstring savePath, uint32 index /*= 0*/)
{
	std::wstring finalPath = m_modelPath + savePath + L".anim";
	assert(index < m_scene->mNumAnimations);

	std::shared_ptr<AssimpConvert::Animation> animation = ReadAnimationData(m_scene->mAnimations[index]);
	ReadSkinnedModelData(m_scene->mRootNode, -1, -1, false);	// bone만 불러온다.
	WriteAnimationData(animation, finalPath);
}

void AssimpConverter::ExportVertexPositionData(const std::wstring& savePath)
{
	std::wstring finalPath = m_modelPath + savePath + L".pos";
	WriteVertexPositionFile(finalPath);
}

std::string AssimpConverter::WriteTexture(std::string SaveFolder, std::string File)
{
	std::string fileName = std::filesystem::path(File).filename().string();
	std::string folderName = std::filesystem::path(SaveFolder).filename().string();

	const aiTexture* srcTexture = m_scene->GetEmbeddedTexture(File.c_str());

	// 텍스쳐가 내장되어있을 경우
	if (srcTexture)
	{
		std::string pathStr = (std::filesystem::path(SaveFolder) / fileName).string();

		if (srcTexture->mHeight == 0)
		{
			std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
			file->Open(ConvertStringToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
	}
	// 텍스쳐가 따로 있을 경우
	else
	{
		// 원래있던 파일을 내가 원하는 경로로 옮긴다.
		if (File.size() > 0)
		{

			std::string originStr = (std::filesystem::path(m_assetPath) / folderName / File).string();
			Replace(originStr, "\\", "/");

			std::string pathStr = (std::filesystem::path(SaveFolder) / fileName).string();
			Replace(pathStr, "\\", "/");

			bool isSuccess = ::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
			if (!isSuccess)
			{
				assert(false);
				MessageBox(NULL, L"원본 텍스쳐가 없습니다.", L"AssimpConverter", MB_OK);
			}
		}
	}
	return fileName;
}

void AssimpConverter::WriteMaterialData(std::wstring FilePath)
{
	auto path = std::filesystem::path(FilePath);

	std::filesystem::create_directory(path.parent_path());

	std::string folder = path.parent_path().string();
	//folder += "/";
	std::shared_ptr<tinyxml2::XMLDocument> document = std::make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (std::shared_ptr<AssimpConvert::Material> material : m_materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseTextureFile).c_str());
		node->LinkEndChild(element);
		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->specularTextureFile).c_str());
		node->LinkEndChild(element);
		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->normalTextureFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	std::string filePathString = std::string().assign(FilePath.begin(), FilePath.end());
	document->SaveFile(filePathString.c_str());
}

void AssimpConverter::WriteModelFile(const std::wstring& filePath)
{
	auto path = std::filesystem::path(filePath);

	std::filesystem::create_directory(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(filePath, FileMode::Write);

	// Bone Data
	file->Write<uint32>((uint32)m_bones.size());
	for (auto& bone : m_bones)
	{
		file->Write<int32>(bone->index);
		file->Write<std::string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}
	file->Write<uint32>((uint32)m_meshes.size());
	for (auto& mesh : m_meshes)
	{
		file->Write<std::string>(mesh->name);
		file->Write<int32>(mesh->boneIndex);
		file->Write<std::string>(mesh->materialName);

		// vertex
		file->Write<uint32>((uint32)mesh->vertices.size());
		file->Write(&mesh->vertices[0], sizeof(BasicVertex) * (uint32)mesh->vertices.size());

		// index
		file->Write<uint32>((uint32)mesh->indices.size());
		file->Write(&mesh->indices[0], sizeof(uint32) * (uint32)mesh->indices.size());

	}
}

void AssimpConverter::WriteSkinnedModelFile(const std::wstring& filePath)
{
	auto path = std::filesystem::path(filePath);

	std::filesystem::create_directory(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(filePath, FileMode::Write);

	// Bone Data
	file->Write<uint32>((uint32)m_bones.size());
	for (auto& bone : m_bones)
	{
		file->Write<int32>(bone->index);
		file->Write<std::string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}
	file->Write<uint32>((uint32)m_skinnedMeshes.size());
	for (auto& mesh : m_skinnedMeshes)
	{
		file->Write<std::string>(mesh->name);
		file->Write<int32>(mesh->boneIndex);
		file->Write<std::string>(mesh->materialName);

		// vertex
		file->Write<uint32>((uint32)mesh->vertices.size());
		file->Write(&mesh->vertices[0], sizeof(SkinnedVertex) * (uint32)mesh->vertices.size());

		// index
		file->Write<uint32>((uint32)mesh->indices.size());
		file->Write(&mesh->indices[0], sizeof(uint32) * (uint32)mesh->indices.size());

	}
}

void AssimpConverter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
	std::shared_ptr<AssimpConvert::Bone> bone = std::make_shared<AssimpConvert::Bone>();
	bone->index = index;
	bone->parent = parent;
	bone->name = node->mName.C_Str();


	/// Relative Transform
	// float 첫번째 주소 값으로 matrix 복사
	Matrix transform(node->mTransformation[0]);
	bone->transform = transform.Transpose();

	// root (local)
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = m_bones[parent]->transform;

	// Local Transform
	bone->transform = bone->transform * matParent;

	m_bones.push_back(bone);

	ReadMeshData(node, index);

	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		ReadModelData(node->mChildren[i], (uint32)m_bones.size(), index);
	}
}

void AssimpConverter::ReadSkinnedModelData(aiNode* node, int32 index, int32 parent, bool readMeshData /*= true*/)
{
	std::shared_ptr<AssimpConvert::Bone> bone = std::make_shared<AssimpConvert::Bone>();
	bone->index = index;
	bone->parent = parent;
	bone->name = node->mName.C_Str();


	/// Relative Transform
	// float 첫번째 주소 값으로 matrix 복사
	Matrix transform(node->mTransformation[0]);
	bone->transform = transform.Transpose();

	// root (local)
	Matrix matParent = Matrix::Identity;
	if (parent >= 0)
		matParent = m_bones[parent]->transform;

	// Local Transform
	bone->transform = bone->transform * matParent;

	m_bones.push_back(bone);

	// Mesh까지 읽을 경우
	if (readMeshData)
	{
		ReadSkinnedMeshData(node, index);
	}

	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		ReadSkinnedModelData(node->mChildren[i], (uint32)m_bones.size(), index);
	}
}

void AssimpConverter::ReadMaterialData()
{
	for (uint32 i = 0; i < m_scene->mNumMaterials; ++i)
	{
		aiMaterial* srcMaterial = m_scene->mMaterials[i];

		std::shared_ptr<AssimpConvert::Material> material = std::make_shared<AssimpConvert::Material>();
		material->name = srcMaterial->GetName().C_Str();

		aiColor3D color;

		// Ambient
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = Color(color.r, color.g, color.b, 1.f);

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = Color(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = Color(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);

		// Emissive
		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->emissive = Color(color.r, color.g, color.b, 1.f);

		//----------------Texture----------------//
		aiString file;
		/*if (srcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), file) == AI_SUCCESS)
		{
			int a = 3;
		}*/
		// Diffuse Texture
		if (srcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), file) == AI_SUCCESS)
		{
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
			material->diffuseTextureFile = file.C_Str();
		}
		// Specular Texture
		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->specularTextureFile = file.C_Str();

		// Emissive Texture
		srcMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &file);
		material->emissiveTextureFile = file.C_Str();

		// Normal Texture
		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->normalTextureFile = file.C_Str();

		m_materials.push_back(material);
	}
}

void AssimpConverter::ReadSkinData()
{
	for (uint32 i = 0; i < m_scene->mNumMeshes; ++i)
	{
		aiMesh* srcMesh = m_scene->mMeshes[i];
		if (srcMesh->HasBones() == false)
		{
			continue;
		}

		std::shared_ptr<AssimpConvert::SkinnedMesh> mesh = m_skinnedMeshes[i];

		std::vector<AssimpConvert::BoneWeights> tempVertexBoneWeights;
		tempVertexBoneWeights.resize(mesh->vertices.size());

		// bone을 순회하면서 연관된 vertexId, Weight를 구해서 기록
		for (uint32 b = 0; b < srcMesh->mNumBones; b++)
		{
			aiBone* srcMeshBone = srcMesh->mBones[b];
			uint32 boneIndex = GetBoneIndex(srcMeshBone->mName.C_Str());

			for (uint32 w = 0; w < srcMeshBone->mNumWeights; w++)
			{
				uint32 index = srcMeshBone->mWeights[w].mVertexId;
				float weight = srcMeshBone->mWeights[w].mWeight;

				tempVertexBoneWeights[index].AddWeights(boneIndex, weight);
			}
		}

		// 최종 결과
		for (uint32 v = 0; v < tempVertexBoneWeights.size(); ++v)
		{
			tempVertexBoneWeights[v].Normalize();

			AssimpConvert::BlendWeight blendWeight = tempVertexBoneWeights[v].GetBlendWeights();
			mesh->vertices[v].BlendIndices = blendWeight.indices;
			mesh->vertices[v].BlendWeights = blendWeight.weights;
		}
	}
}

void AssimpConverter::ReadMeshData(aiNode* node, int32 bone)
{
	// 마지막 노드는 정보를 들고 있다.
	if (node->mNumMeshes < 1)
	{
		return;
	}

	std::shared_ptr<AssimpConvert::Mesh> mesh = std::make_shared<AssimpConvert::Mesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;

	// submesh
	for (uint32 i = 0; i < node->mNumMeshes; ++i)
	{
		uint32 index = node->mMeshes[i];
		const aiMesh* srcMesh = m_scene->mMeshes[index];

		// Material 이름
		const aiMaterial* material = m_scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		// mesh가 여러개일 경우 index가 중복될 수 있다. 
		// 하나로 관리하기 위해 미리 이전 vertex의 size를 가져와서 이번에 추가하는 index에 더해 중복을 피한다.

		const uint32 startVertex = (uint32)mesh->vertices.size();

		// Vertex
		for (uint32 v = 0; v < srcMesh->mNumVertices; ++v)
		{
			BasicVertex vertex;
			{
				memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
			}

			// UV
			if (srcMesh->HasTextureCoords(0))
			{
				memcpy(&vertex.UV, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));
			}

			// Normal
			if (srcMesh->HasNormals())
			{
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));
			}

			// Tangent
			if (srcMesh->HasTangentsAndBitangents())
			{
				memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));
			}

			mesh->vertices.push_back(vertex);
		}

		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; ++k)
			{
				mesh->indices.push_back(face.mIndices[k] + startVertex);
			}
		}
	}
	m_meshes.push_back(mesh);
}

void AssimpConverter::ReadSkinnedMeshData(aiNode* node, int32 bone)
{
	// 마지막 노드는 정보를 들고 있다.
	if (node->mNumMeshes < 1)
	{
		return;
	}

	std::shared_ptr<AssimpConvert::SkinnedMesh> mesh = std::make_shared<AssimpConvert::SkinnedMesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;

	// submesh
	for (uint32 i = 0; i < node->mNumMeshes; ++i)
	{
		uint32 index = node->mMeshes[i];
		const aiMesh* srcMesh = m_scene->mMeshes[index];

		// Material 이름
		const aiMaterial* material = m_scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		// mesh가 여러개일 경우 index가 중복될 수 있다. 
		// 하나로 관리하기 위해 미리 이전 vertex의 size를 가져와서 이번에 추가하는 index에 더해 중복을 피한다.

		const uint32 startVertex = (uint32)mesh->vertices.size();

		// Vertex
		for (uint32 v = 0; v < srcMesh->mNumVertices; ++v)
		{
			SkinnedVertex vertex;
			{
				memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
			}

			// UV
			if (srcMesh->HasTextureCoords(0))
			{
				memcpy(&vertex.UV, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));
			}

			// Normal
			if (srcMesh->HasNormals())
			{
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));
			}

			// Tangent
			if (srcMesh->HasTangentsAndBitangents())
			{
				memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));
			}

			mesh->vertices.push_back(vertex);
		}

		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; ++k)
			{
				mesh->indices.push_back(face.mIndices[k] + startVertex);
			}
		}
	}
	m_skinnedMeshes.push_back(mesh);
}

std::shared_ptr<AssimpConvert::Animation> AssimpConverter::ReadAnimationData(aiAnimation* srcAnimation)
{
	std::shared_ptr<AssimpConvert::Animation> animation = std::make_shared<AssimpConvert::Animation>();
	animation->name = srcAnimation->mName.C_Str();
	animation->frameRate = (float)srcAnimation->mTicksPerSecond;
	animation->frameCount = (uint32)srcAnimation->mDuration + 1;
	
	// Animation의 본 개수
	animation->numBones = srcAnimation->mNumChannels;

	std::map<std::string, std::shared_ptr<AssimpConvert::AnimationNode>> cacheAnimNode;

	for (uint32 i = 0; i < srcAnimation->mNumChannels; ++i)
	{
		aiNodeAnim* srcNode = srcAnimation->mChannels[i];

		std::shared_ptr<AssimpConvert::AnimationNode> node = ParseAnimationNode(animation, srcNode);

		animation->duration = max(animation->duration, node->keyframe.back().time);

		cacheAnimNode[srcNode->mNodeName.C_Str()] = node;
	}

	ReadKeyFrameData(animation, m_scene->mRootNode, cacheAnimNode);

	return animation;
}

std::shared_ptr<AssimpConvert::AnimationNode> AssimpConverter::ParseAnimationNode(std::shared_ptr<AssimpConvert::Animation> animation, aiNodeAnim* srcNode)
{
	std::shared_ptr<AssimpConvert::AnimationNode> node = std::make_shared<AssimpConvert::AnimationNode>();
	node->name = srcNode->mNodeName;

	// 그냥 최대 키 카운트로 설정
	uint32 keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumScalingKeys), srcNode->mNumRotationKeys);

	for (uint32 k = 0; k < keyCount; ++k)
	{
		AssimpConvert::KeyFrameData frameData;

		bool found = false;
		uint32 t = (uint32)node->keyframe.size();

		// position
		if (fabsf((float)srcNode->mPositionKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mPositionKeys[k];
			frameData.time = (float)key.mTime;
			frameData.translation.x = key.mValue.x;
			frameData.translation.y = key.mValue.y;
			frameData.translation.z = key.mValue.z;

			found = true;
		}

		// Rotation
		if (fabsf((float)srcNode->mRotationKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiQuatKey key = srcNode->mRotationKeys[k];
			frameData.time = (float)key.mTime;

			frameData.rotation.x = key.mValue.x;
			frameData.rotation.y = key.mValue.y;
			frameData.rotation.z = key.mValue.z;
			frameData.rotation.w = key.mValue.w;

			found = true;
		}

		// Scale
		if (fabs((float)srcNode->mScalingKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mScalingKeys[k];
			frameData.time = (float)key.mTime;

			frameData.scale.x = key.mValue.x;
			frameData.scale.y = key.mValue.y;
			frameData.scale.z = key.mValue.z;

			found = true;
		}

		if (found == true)
		{
			node->keyframe.push_back(frameData);
		}
	}

	// keyframe 늘려주는 곳
	if (node->keyframe.size() < animation->frameCount)
	{
		uint32 count = animation->frameCount - (uint32)node->keyframe.size();
		AssimpConvert::KeyFrameData keyFrame = node->keyframe.back();

		for (uint32 n = 0; n < count; ++n)
		{
			node->keyframe.push_back(keyFrame);
		}
	}
	return node;
}

void AssimpConverter::ReadKeyFrameData(std::shared_ptr<AssimpConvert::Animation> animation, aiNode* srcNode, std::map<std::string, std::shared_ptr<AssimpConvert::AnimationNode>>& cache)
{
	std::shared_ptr<AssimpConvert::KeyFrame> keyFrame = std::make_shared<AssimpConvert::KeyFrame>();
	keyFrame->boneName = srcNode->mName.C_Str();

	std::shared_ptr<AssimpConvert::AnimationNode> findNode = cache[srcNode->mName.C_Str()];

	for (uint32 i = 0; i < animation->frameCount; ++i)
	{
		AssimpConvert::KeyFrameData frameData;

		if (findNode == nullptr)
		{
			Matrix transform(srcNode->mTransformation[0]);
			transform = transform.Transpose();
			frameData.time = (float)i;
			transform.Decompose(OUT frameData.scale, OUT frameData.rotation, OUT frameData.translation);
		}
		else
		{
			frameData = findNode->keyframe[i];
		}

		keyFrame->transforms.push_back(frameData);
	}

	animation->keyFrames.push_back(keyFrame);

	for (uint32 i = 0; i < srcNode->mNumChildren; ++i)
	{
		ReadKeyFrameData(animation, srcNode->mChildren[i], cache);
	}
}

void AssimpConverter::WriteAnimationData(std::shared_ptr<AssimpConvert::Animation> animation, std::wstring finalPath)
{
	auto path = std::filesystem::path(finalPath);

	std::filesystem::create_directory(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(finalPath, FileMode::Write);

	file->Write<std::string>(animation->name);
	file->Write<float>(animation->duration);
	file->Write<float>(animation->frameRate);
	file->Write<uint32>(animation->frameCount);

	file->Write<uint32>((uint32)animation->keyFrames.size());

	for (std::shared_ptr<AssimpConvert::KeyFrame> keyFrame : animation->keyFrames)
	{
		file->Write<std::string>(keyFrame->boneName);
		file->Write<uint32>((uint32)keyFrame->transforms.size());
		file->Write(&keyFrame->transforms[0], sizeof(AssimpConvert::KeyFrameData) * (uint32)keyFrame->transforms.size());
	}

	//file->Write<int32>(animation->numBones);
	// Bone Data
	file->Write<uint32>((uint32)m_bones.size());
	for (auto& bone : m_bones)
	{
		file->Write<int32>(bone->index);
		file->Write<std::string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}
}

uint32 AssimpConverter::GetBoneIndex(const std::string& name)
{
	for (std::shared_ptr<AssimpConvert::Bone>& bone : m_bones)
	{
		if (bone->name == name)
		{
			return bone->index;
		}
	}
	assert(false);
	MessageBox(NULL, L"Cant find Bone Index", L"AssimpConverter::GetBoneIndex", MB_OK);
	return 0;
}

void AssimpConverter::WriteVertexPositionFile(const std::wstring& filePath)
{
	auto path = std::filesystem::path(filePath);

	std::filesystem::create_directory(path.parent_path());

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(filePath, FileMode::Write);
	
	// 매쉬 개수
	file->Write<uint32>((uint32)m_meshes.size());

	for (auto& mesh : m_meshes)
	{
		// vertex 개수
		file->Write<uint32>((uint32)mesh->vertices.size());
		//file->Write(&mesh->vertices[0], sizeof(BasicVertex) * (uint32)mesh->vertices.size());

		for (uint32 i = 0; i < (uint32)mesh->vertices.size(); ++i)
		{
			file->Write<float>(mesh->vertices[i].Position.x);
			file->Write<float>(mesh->vertices[i].Position.y);
			file->Write<float>(mesh->vertices[i].Position.z);
		}
	}
}
