#include "GraphicsEngine/Resource/Model.h"
#include "Misc/Utils/tinyxml2.h"
#include <filesystem>
#include "Misc/Utils/StringUtils.h"

#include "GraphicsEngine/Resource/Bone.h"
#include "GraphicsEngine/Resource/Mesh.h"
#include "GraphicsEngine/Resource/Material.h"
#include "GraphicsEngine/Resource/ModelAnimation.h"

#include "Misc/Utils/FileUtils.h"

#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"



Ideal::Model::Model()
{

}

Ideal::Model::~Model()
{

}

void Ideal::Model::SetTransformMatrix(DirectX::SimpleMath::Matrix Matrix)
{
	m_transformMatrix = Matrix;
	for (auto m : m_meshes)
	{
		m->SetTransformMatrix(m_transformMatrix);
	}
}

void Ideal::Model::ReadMaterial(const std::wstring& filename)
{
	//std::wstring texturePath = L"Resources/Textures/";
	std::wstring fullPath = filename + L".xml";

	std::filesystem::path parentPath = std::filesystem::path(fullPath).parent_path();

	//tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
	std::shared_ptr<tinyxml2::XMLDocument> document = std::make_shared<tinyxml2::XMLDocument>();
	tinyxml2::XMLError error = document->LoadFile(StringUtils::ConvertWStringToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);
	if (error != tinyxml2::XML_SUCCESS)
	{
		MessageBox(NULL, fullPath.c_str(), L"read material", MB_OK);
	}

	tinyxml2::XMLElement* root = document->FirstChildElement();
	tinyxml2::XMLElement* materialNode = root->FirstChildElement();

	while (materialNode)
	{
		std::shared_ptr<Ideal::Material> material = std::make_shared<Ideal::Material>();

		tinyxml2::XMLElement* node = nullptr;

		node = materialNode->FirstChildElement();
		material->SetName((node->GetText()));

		// DiffuseTexture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				// Temp 2024.04.20
				std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;

				material->m_diffuseTextureFile = finalTextureStr;
				//material->m_diffuseTextureFile = textureStr;
				
				// TODO : Texture만들기
				//auto texture
				// TEMP : 2024.04.19 임시로 여기다가 텍스쳐를 만들겠다.
			}
		}

		// Specular Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				material->m_specularTextureFile = textureStr;
				// TODO : Texture만들기
				//auto texture
			}
		}

		// Normal Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				material->m_normalTextureFile = textureStr;
				// TODO : Texture만들기
				//auto texture
			}
		}

		// Ambient
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->SetAmbient(color);
		}

		// Diffuse
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->SetDiffuse(color);
		}

		// Specular
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->SetSpecular(color);
		}

		// Emissive
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");
			material->SetEmissive(color);
		}

		m_materials.push_back(material);

		materialNode = materialNode->NextSiblingElement();
	}
	BindCacheInfo();
}

void Ideal::Model::ReadModel(const std::wstring& filename)
{
	//std::wstring fullPath = L"Resources/Models/";
	//fullPath += filename + L".mesh";

	std::wstring fullPath = filename + L".mesh";
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	// bone
	{
		const uint32 count = file->Read<uint32>();
		
		for (uint32 i = 0; i < count; ++i)
		{
			std::shared_ptr<Ideal::Bone> bone = std::make_shared<Ideal::Bone>();
			bone->index = file->Read<int32>();
			bone->name = file->Read<std::string>();
			bone->parent = file->Read<int32>();
			bone->transform = file->Read<Matrix>();
			m_bones.push_back(bone);
		}
	}

	// Mesh
	{
		const uint32 count = file->Read<uint32>();

		for (uint32 i = 0; i < count; ++i)
		{
			std::shared_ptr <Ideal::Mesh> mesh = std::make_shared<Ideal::Mesh>();

			mesh->SetName(file->Read<std::string>());
			mesh->m_boneIndex = file->Read<int32>();

			// Material
			mesh->m_materialName = file->Read<std::string>();

			// vertex data
			{
				const uint32 count = file->Read<uint32>();
				std::vector<BasicVertex> vertices;
				vertices.resize(count);

				void* data = vertices.data();
				file->Read(&data, sizeof(BasicVertex) * count);
				mesh->AddVertices(vertices);
			}

			// index Data
			{
				const uint32 count = file->Read<uint32>();
				std::vector<uint32> indices;
				indices.resize(count);

				void* data = indices.data();
				file->Read(&data, sizeof(uint32) * count);
				mesh->AddIndices(indices);
			}

			// TODO : mesh Create
			//mesh->Create()
			m_meshes.push_back(mesh);
		}
	}
	BindCacheInfo();
}

void Ideal::Model::ReadAnimation(const std::wstring& filename)
{
	std::wstring fullPath = filename + L".anim";
	
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	std::shared_ptr<Ideal::ModelAnimation> animation = std::make_shared<Ideal::ModelAnimation>();

	animation->name = StringUtils::ConvertStringToWString(file->Read<std::string>());
	animation->duration = file->Read<float>();
	animation->frameRate = file->Read<float>();
	animation->frameCount = file->Read<uint32>();

	uint32 keyframeCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframeCount; ++i)
	{
		std::shared_ptr<ModelKeyframe> keyFrame = std::make_shared<ModelKeyframe>();
		keyFrame->boneName = StringUtils::ConvertStringToWString(file->Read<std::string>());

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyFrame->transforms.resize(size);
			void* ptr = &keyFrame->transforms[0];
			file->Read(&ptr, sizeof(Ideal::ModelKeyFrameData) * size);
		}

		animation->keyframes[keyFrame->boneName] = keyFrame;
	}

	m_animations.push_back(animation);
}

void Ideal::Model::Create(std::shared_ptr<D3D12Renderer> Renderer)
{
	for (auto& m : m_meshes)
	{
		m->Create(Renderer);
	}
}

void Ideal::Model::Tick(std::shared_ptr<D3D12Renderer> Renderer)
{
	for (auto& m : m_meshes)
	{
		m->Tick(Renderer);
	}
}

void Ideal::Model::Render(std::shared_ptr<D3D12Renderer> Renderer)
{
	for (auto& m : m_meshes)
	{
		m->Render(Renderer);
	}
}

void Ideal::Model::BindCacheInfo()
{
	for (const auto& mesh : m_meshes)
	{
		if (mesh->m_material != nullptr)
		{
			continue;
		}

		mesh->m_material = GetMaterialByName(mesh->m_materialName);
	}

	//for (const auto& mesh : m_meshes)
	//{
	//	if(mesh->m_bone != nullptr)
	//}
}

std::shared_ptr<Ideal::Material> Ideal::Model::GetMaterialByName(const std::string& name)
{
	for (auto& m : m_materials)
	{
		if (m->GetName() == name)
		{
			return m;
		}
	}
	return nullptr;
}

std::shared_ptr<Ideal::Mesh> Ideal::Model::GetMeshByName(const std::string& name)
{
	for (auto& m : m_meshes)
	{
		if (m->GetName() == name)
		{
			return m;
		}
	}
	return nullptr;
}

std::shared_ptr<Ideal::Bone> Ideal::Model::GetBoneByName(const std::string& name)
{
	for (auto& m : m_bones)
	{
		if (m->GetName() == name)
		{
			return m;
		}
	}
	return nullptr;
}

std::shared_ptr<Ideal::ModelAnimation> Ideal::Model::GetAnimationByIndex(uint32 index)
{
	 return (index < 0 || index >= m_animations.size() ? nullptr : m_animations[index]); 
}

std::shared_ptr<Ideal::ModelAnimation> Ideal::Model::GetAnimationByName(const std::wstring& name)
{
	for (auto& animation : m_animations)
	{
		if (animation->name == name)
			return animation;
	}
	return nullptr;
}
