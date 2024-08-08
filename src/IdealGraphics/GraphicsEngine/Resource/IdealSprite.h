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
	class D3D12DescriptorHeap;
	template <typename> class IdealMesh;
	class ITexture;
}

namespace Ideal
{
	class IdealSprite : public ISprite
	{
	public:
		IdealSprite();
		virtual ~IdealSprite();

	public:
		// device, cb pool, commandlist 필요
		void DrawSprite(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> UIDescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);
		// ComPtr<ID3D12Device> Device, ComPtr<ID3D12CommandList> CommandList, std::shared_ptr<Ideal::D3D12ConstantBufferPool> ConstantBufferPool
	public:
		//---Interface---//
		virtual Vector2 const& GetPosition() override { return m_cbSprite.Pos; }
		virtual Vector2 const& GetSamplePosition() override { return m_cbSprite.TexSamplePos; }
		virtual Vector2 const& GetSampleSize() override { return m_cbSprite.TexSampleSize; }
		virtual float GetZ() override { return m_cbSprite.Z; }
		virtual float GetAlpha() override { return m_cbSprite.Alpha; }

		virtual void SetScreenPosition(const Vector2& ScreenPos) override;
		virtual void SetPosition(const Vector2& Position) override;
		virtual void SetTextureSize(const Vector2& TextureSize) override;
		virtual void SetTextureSamplePosition(const Vector2& TextureSamplePosition) override;
		virtual void SetTextureSampleSize(const Vector2& TextureSampleSize) override;
		virtual void SetZ(float Z) override;
		virtual void SetAlpha(float Alpha) override;

		virtual void SetTexture(std::weak_ptr<Ideal::ITexture> Texture) override;

	public:
		// default mesh를 넣어줄 것
		void SetMesh(std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> Mesh);

	private:
		// 어차피 default mesh를 쓸건데 weak ptr이면 될 것 같음
		std::weak_ptr<Ideal::IdealMesh<SimpleVertex>> m_mesh;

		// 텍스쳐를 가지고 있는다...
		std::weak_ptr<Ideal::D3D12Texture> m_texture;
		// 스프라이트 정보!
		CB_Sprite m_cbSprite;
	};
}
