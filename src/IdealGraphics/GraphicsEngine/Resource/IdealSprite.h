#pragma once
#include "ISprite.h"
#include "GraphicsEngine/ConstantBufferInfo.h"
#include "GraphicsEngine/VertexInfo.h"
#include <memory>

struct ID3D12Device;
struct ID3D12CommandList;

namespace Ideal
{
	class D3D12Texture;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12DynamicDescriptorHeap;
	template <typename> class IdealMesh;
}

namespace Ideal
{
	class IdealSprite : public ISprite
	{
	public:
		IdealSprite();
		virtual ~IdealSprite();

	public:
		// device, cb pool, commandlist �ʿ�
		void DrawSprite(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DynamicDescriptorHeap> UIDescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);
		// ComPtr<ID3D12Device> Device, ComPtr<ID3D12CommandList> CommandList, std::shared_ptr<Ideal::D3D12ConstantBufferPool> ConstantBufferPool
	public:
		//---Interface---//
		Vector2 const& GetPosition() { return m_cbSprite.Pos; }
		Vector2 const& GetSamplePosition() { return m_cbSprite.TexSamplePos; }
		Vector2 const& GetSampleSize() { return m_cbSprite.TexSampleSize; }
		float GetZ() { return m_cbSprite.Z; }
		float GetAlpha() { return m_cbSprite.Alpha; }

		void SetScreenPosition(Vector2 ScreenPos);
		void SetPosition(Vector2 Position);
		void SetTextureSize(Vector2 TextureSize);
		void SetTextureSamplePosition(Vector2 TextureSamplePosition);
		void SetTextureSampleSize(Vector2 TextureSampleSize);
		void SetZ(float Z);
		void SetAlpha(float Alpha);

		void SetTexture(std::weak_ptr<Ideal::D3D12Texture> Texture);

	public:
		// default mesh�� �־��� ��
		void SetMesh(std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> Mesh);

	private:
		// ������ default mesh�� ���ǵ� weak ptr�̸� �� �� ����
		std::weak_ptr<Ideal::IdealMesh<SimpleVertex>> m_mesh;

		// �ؽ��ĸ� ������ �ִ´�...
		std::weak_ptr<Ideal::D3D12Texture> m_texture;
		// ��������Ʈ ����!
		CB_Sprite m_cbSprite;
	};
}
