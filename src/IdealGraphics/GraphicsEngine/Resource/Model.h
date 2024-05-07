#pragma once
#include "GraphicsEngine/Resource/ResourceBase.h"
#include "Core/Core.h"

//class IdealRenderer;

namespace Ideal
{
	class Material;
	class Mesh;
	class Bone;
	struct ModelAnimation;
}

struct ID3D12GraphicsCommandList;
class D3D12Renderer;

namespace Ideal
{
	class Model : public ResourceBase
	{
	public:
		Model();
		virtual ~Model();

	public:
		void SetTransformMatrix(DirectX::SimpleMath::Matrix Matrix);

	public:
		void ReadMaterial(const std::wstring& filename);
		void ReadModel(const std::wstring& filename);
		void ReadAnimation(const std::wstring& filename);

		void Create(std::shared_ptr<D3D12Renderer> Renderer);
		void Tick(std::shared_ptr<D3D12Renderer> Renderer);
		void Render(std::shared_ptr<D3D12Renderer> Renderer);

	public:
		uint32 GetBoneCount() { return static_cast<uint32>(m_bones.size()); }
		std::shared_ptr<Ideal::Bone>			GetBoneByIndex(uint32 i) { return m_bones[i]; }
		std::vector<std::shared_ptr<Ideal::Mesh>>& GetMeshes() { return m_meshes; }
		std::shared_ptr<Ideal::Material>		GetMaterialByName(const std::string& name);
		std::shared_ptr<Ideal::Mesh>			GetMeshByName(const std::string& name);
		std::shared_ptr<Ideal::Bone>			GetBoneByName(const std::string& name);
		std::shared_ptr<Ideal::ModelAnimation>	GetAnimationByIndex(uint32 index);
		std::shared_ptr<Ideal::ModelAnimation> GetAnimationByName(const std::string& name);
		void BindCacheInfo();
	private:
		std::vector<std::shared_ptr<Ideal::Material>> m_materials;
		std::vector<std::shared_ptr<Ideal::Bone>> m_bones;
		std::vector<std::shared_ptr<Ideal::Mesh>> m_meshes;
		std::vector<std::shared_ptr<Ideal::ModelAnimation>> m_animations;

	private:
		DirectX::SimpleMath::Matrix m_transformMatrix;
	};
}

