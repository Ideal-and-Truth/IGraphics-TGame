#include "GraphicsEngine/D3D12/ResourceManager.h"

#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/Refactor/IdealSkinnedMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealSkinnedMeshObject.h"

#include "GraphicsEngine/Resource/Refactor/IdealBone.h"
#include "GraphicsEngine/Resource/Refactor/IdealMesh.h"
#include "GraphicsEngine/Resource/Refactor/IdealMaterial.h"
#include "GraphicsEngine/Resource/Refactor/IdealAnimation.h"
#include "ThirdParty/Include/DirectXTK12/WICTextureLoader.h"
#include "Misc/Utils/FileUtils.h"
#include "Misc/Utils/StringUtils.h"
#include "Misc/Utils/tinyxml2.h"
#include "GraphicsEngine/Resource/ModelAnimation.h"

#include <filesystem>

using namespace Ideal;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	WaitForFenceValue();
}

void ResourceManager::Init(ComPtr<ID3D12Device> Device)
{
	m_device = Device;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	Check(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

	//-----------Create Command Allocator---------//
	Check(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
	m_commandAllocator->SetName(L"ResourceAllocator");

	//-----------Create Command List---------//
	Check(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.GetAddressOf())));
	m_commandList->Close();

	//-----------Create Fence---------//
	Check(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
	m_fenceValue = 0;
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	m_fence->SetName(L"ResourceManager Fence");

	//------------SRV Descriptor-----------//
	m_srvHeap.Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_srvHeapCount);
}

void ResourceManager::Fence()
{
	m_fenceValue++;
	m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
}

void ResourceManager::WaitForFenceValue()
{
	const uint64 expectedFenceValue = m_fenceValue;

	if (m_fence->GetCompletedValue() < expectedFenceValue)
	{
		m_fence->SetEventOnCompletion(expectedFenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void ResourceManager::CreateVertexBufferBox(std::shared_ptr<Ideal::D3D12VertexBuffer>& VertexBuffer)
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	ComPtr<ID3D12Resource> vertexBufferUpload = nullptr;	// 업로드 용으로 버퍼 하나를 만드는 것 같다.
	const VertexTest2 cubeVertices[] = {
		{ { -0.5f, 0.5f, -0.5f},	Vector2(0.0f, 1.0f) },    // Back Top Left
		{ {  0.5f, 0.5f, -0.5f},	Vector2(0.0f, 0.0f) },    // Back Top Right
		{ {  0.5f, 0.5f,  0.5f},	Vector2(1.0f, 0.0f) },    // Front Top Right
		{ { -0.5f, 0.5f,  0.5f},	Vector2(1.0f, 1.0f)	},    // Front Top Left

		{ { -0.5f, -0.5f,-0.5f},	Vector2(0.0f, 1.0f)	},    // Back Bottom Left
		{ {  0.5f, -0.5f,-0.5f},	Vector2(0.0f, 0.0f) },    // Back Bottom Right
		{ {  0.5f, -0.5f, 0.5f},	Vector2(1.0f, 0.0f) },    // Front Bottom Right
		{ { -0.5f, -0.5f, 0.5f},	Vector2(1.0f, 1.0f) },    // Front Bottom Left
	};
	const uint32 vertexBufferSize = sizeof(cubeVertices);

	// UploadBuffer를 만든다.
	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), vertexBufferSize);

	void* mappedUploadHeap = uploadBuffer.Map();
	memcpy(mappedUploadHeap, cubeVertices, sizeof(cubeVertices));
	uploadBuffer.UnMap();

	uint32 vertexTypeSize = sizeof(VertexTest2);
	uint32 vertexCount = _countof(cubeVertices);
	VertexBuffer->Create(m_device.Get(), m_commandList.Get(), vertexTypeSize, vertexCount, uploadBuffer);

	// close
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	VertexBuffer->SetName(L"TestBoxVB");

	Fence();
	WaitForFenceValue();
}

void ResourceManager::CreateIndexBufferBox(std::shared_ptr<Ideal::D3D12IndexBuffer> IndexBuffer)
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	uint32 indices[] = {
			0, 1, 3,
			1, 2, 3,

			3, 2, 7,
			6, 7, 2,

			2, 1, 6,
			5, 6, 1,

			1, 0, 5,
			4, 5, 0,

			0, 3, 4,
			7, 4, 3,

			7, 6, 4,
			5, 4, 6, };
	const uint32 indexBufferSize = sizeof(indices);

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), indexBufferSize);


	// 업로드버퍼에 먼저 복사
	void* mappedUploadBuffer = uploadBuffer.Map();
	memcpy(mappedUploadBuffer, indices, indexBufferSize);
	uploadBuffer.UnMap();

	uint32 indexTypeSize = sizeof(uint32);
	uint32 indexCount = _countof(indices);

	IndexBuffer->Create(m_device.Get(), m_commandList.Get(), indexTypeSize, indexCount, uploadBuffer);

	//-----------Execute-----------//
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	IndexBuffer->SetName(L"TestBoxIB");

	Fence();
	WaitForFenceValue();
}

void ResourceManager::CreateIndexBuffer(std::shared_ptr<Ideal::D3D12IndexBuffer> OutIndexBuffer, std::vector<uint32>& Indices)
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	const uint32 elementSize = sizeof(uint32);
	const uint32 elementCount = Indices.size();
	const uint32 bufferSize = elementSize * elementCount;

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), bufferSize);
	{
		void* mappedData = uploadBuffer.Map();
		memcpy(mappedData, Indices.data(), bufferSize);
		uploadBuffer.UnMap();
	}
	OutIndexBuffer->Create(m_device.Get(),
		m_commandList.Get(),
		elementSize,
		elementCount,
		uploadBuffer
	);

	//-----------Execute-----------//
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	Fence();
	WaitForFenceValue();
}

void Ideal::ResourceManager::CreateTexture(std::shared_ptr<Ideal::D3D12Texture> OutTexture, const std::wstring& Path)
{
	//----------------------Init--------------------------//
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	ComPtr<ID3D12Resource> resource;
	Ideal::D3D12DescriptorHandle srvHandle;

	//----------------------Load WIC Texture From File--------------------------//

	std::unique_ptr<uint8_t[]> decodeData;
	D3D12_SUBRESOURCE_DATA subResource;
	Check(DirectX::LoadWICTextureFromFile(
		m_device.Get(),
		Path.c_str(),
		resource.ReleaseAndGetAddressOf(),
		decodeData,
		subResource
	));

	//----------------------Upload Buffer--------------------------//

	uint64 bufferSize = GetRequiredIntermediateSize(resource.Get(), 0, 1);

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), bufferSize);

	//----------------------Update Subresources--------------------------//

	UpdateSubresources(
		m_commandList.Get(),
		resource.Get(),
		uploadBuffer.GetResource(),
		0, 0, 1, &subResource
	);

	//----------------------Resource Barrier--------------------------//

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	m_commandList->ResourceBarrier(1, &barrier);

	//----------------------Execute--------------------------//

	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	Fence();
	WaitForFenceValue();

	//----------------------Create Shader Resource View--------------------------//

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//----------------------Allocate Descriptor-----------------------//
	srvHandle = m_srvHeap.Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvHandle.GetCpuHandle();
	m_device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);

	OutTexture->Create(resource, srvHandle);
}

void Ideal::ResourceManager::CreateStaticMeshObject(std::shared_ptr<D3D12Renderer> Renderer, std::shared_ptr<Ideal::IdealStaticMeshObject> OutMesh, const std::wstring& filename)
{
	// 이미 있을 경우
	std::string key = StringUtils::ConvertWStringToString(filename);
	std::shared_ptr<Ideal::IdealStaticMesh> staticMesh = m_staticMeshes[key];

	if (staticMesh != nullptr)
	{
		OutMesh->SetStaticMesh(staticMesh);
		return;
	}
	staticMesh = std::make_shared<Ideal::IdealStaticMesh>();

	// 없으면 StaticMesh를 만들어서 끼워서 넣어주면된다
	{
		std::wstring fullPath = m_modelPath + filename + L".mesh";
		std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
		file->Open(fullPath, FileMode::Read);

		// bone
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr<Ideal::IdealBone> bone = std::make_shared<Ideal::IdealBone>();
				bone->SetBoneIndex(file->Read<int32>());
				bone->SetName(file->Read<std::string>());
				bone->SetParent(file->Read<int32>());
				bone->SetTransform(file->Read<Matrix>());
				staticMesh->AddBone(bone);
			}
		}

		// SubMesh
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr <Ideal::IdealMesh<BasicVertex>> mesh = std::make_shared<Ideal::IdealMesh<BasicVertex>>();

				mesh->SetName(file->Read<std::string>());
				mesh->SetBoneIndex(file->Read<int32>());

				// Material
				mesh->SetMaterialName(file->Read<std::string>());

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

				staticMesh->AddMesh(mesh);
			}
		}

	
	}

	// Material
	{
		std::wstring fullPath = m_texturePath + filename + L".xml";

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
			std::shared_ptr<Ideal::IdealMaterial> material = std::make_shared<Ideal::IdealMaterial>();

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

					material->SetDiffuseTextureFile(finalTextureStr);
				}
			}

			// Specular Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					material->SetSpecularTextureFile(textureStr);
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
					material->SetNormalTextureFile(textureStr);
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

			staticMesh->AddMaterial(material);

			materialNode = materialNode->NextSiblingElement();
		}
	}

	// Binding info
	staticMesh->FinalCreate(Renderer);

	OutMesh->SetStaticMesh(staticMesh);
}

void ResourceManager::CreateDynamicMeshObject(std::shared_ptr<D3D12Renderer> Renderer, std::shared_ptr<Ideal::IdealSkinnedMeshObject> OutMesh, const std::wstring& filename)
{
	// 이미 있을 경우
	std::string key = StringUtils::ConvertWStringToString(filename);
	std::shared_ptr<Ideal::IdealSkinnedMesh> dynamicMesh = m_dynamicMeshes[key];

	if (dynamicMesh != nullptr)
	{
		OutMesh->SetDynamicMesh(dynamicMesh);
		return;
	}
	dynamicMesh = std::make_shared<Ideal::IdealSkinnedMesh>();

	// 없으면 StaticMesh를 만들어서 끼워서 넣어주면된다
	{
		std::wstring fullPath = m_modelPath + filename + L".dmesh";
		std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
		file->Open(fullPath, FileMode::Read);

		// bone
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr<Ideal::IdealBone> bone = std::make_shared<Ideal::IdealBone>();
				bone->SetBoneIndex(file->Read<int32>());
				bone->SetName(file->Read<std::string>());
				bone->SetParent(file->Read<int32>());
				bone->SetTransform(file->Read<Matrix>());
				dynamicMesh->AddBone(bone);
			}
		}

		// SubMesh
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr <Ideal::IdealMesh<SkinnedVertex>> mesh = std::make_shared<Ideal::IdealMesh<SkinnedVertex>>();

				mesh->SetName(file->Read<std::string>());
				mesh->SetBoneIndex(file->Read<int32>());

				// Material
				mesh->SetMaterialName(file->Read<std::string>());

				// vertex data
				{
					const uint32 count = file->Read<uint32>();
					std::vector<SkinnedVertex> vertices;
					vertices.resize(count);

					void* data = vertices.data();
					file->Read(&data, sizeof(SkinnedVertex) * count);
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

				dynamicMesh->AddMesh(mesh);
			}
		}


	}

	// Material
	{
		std::wstring fullPath = m_texturePath + filename + L".xml";

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
			std::shared_ptr<Ideal::IdealMaterial> material = std::make_shared<Ideal::IdealMaterial>();

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

					material->SetDiffuseTextureFile(finalTextureStr);
				}
			}

			// Specular Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					material->SetSpecularTextureFile(textureStr);
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
					material->SetNormalTextureFile(textureStr);
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

			dynamicMesh->AddMaterial(material);

			materialNode = materialNode->NextSiblingElement();
		}
	}

	// Binding info
	dynamicMesh->FinalCreate(Renderer);

	OutMesh->SetDynamicMesh(dynamicMesh);
}

void ResourceManager::CreateAnimation(std::shared_ptr<Ideal::IdealAnimation> OutAnimation, const std::wstring& filename)
{
	std::string key = StringUtils::ConvertWStringToString(filename);
	std::shared_ptr<Ideal::IdealAnimation> animation = m_animations[key];

	if (animation != nullptr)
	{
		OutAnimation = animation;
		return;
	}

	std::wstring fullPath = m_modelPath + filename + L".anim";

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	//OutAnimation->name = StringUtils::ConvertStringToWString(file->Read<std::string>());
	OutAnimation->name = file->Read<std::string>();
	OutAnimation->duration = file->Read<float>();
	OutAnimation->frameRate = file->Read<float>();
	OutAnimation->frameCount = file->Read<uint32>();

	uint32 keyframeCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframeCount; ++i)
	{
		std::shared_ptr<ModelKeyframe> keyFrame = std::make_shared<ModelKeyframe>();
		//keyFrame->boneName = StringUtils::ConvertStringToWString(file->Read<std::string>());
		keyFrame->boneName = file->Read<std::string>();

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyFrame->transforms.resize(size);
			void* ptr = &keyFrame->transforms[0];
			file->Read(&ptr, sizeof(Ideal::ModelKeyFrameData) * size);
		}

		OutAnimation->keyframes[keyFrame->boneName] = keyFrame;
	}
	m_animations[key] = OutAnimation;
}
