#include "IdealSprite.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include <d3d12.h>
#include <d3dx12.h>

Ideal::IdealSprite::IdealSprite()
{

}

Ideal::IdealSprite::~IdealSprite()
{

}

void Ideal::IdealSprite::DrawSprite(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> UIDescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool)
{
	// TODO :
	// Set Descriptor Heap
	// 
	// ConstantBuffer Pool
	// Copy Descriptor Simple
	// 
	// IASetVertexBuffer
	// IASetIndexBuffer
	// DrawIndexedInstanced
	const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = m_mesh.lock()->GetVertexBufferView();
	const D3D12_INDEX_BUFFER_VIEW& indexBufferView = m_mesh.lock()->GetIndexBufferView();

	CommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	CommandList->IASetIndexBuffer(&indexBufferView);

	// Bind to Shader
	{
		// SRV
		if (!m_texture.expired())
		{
			auto handle = UIDescriptorHeap->Allocate();
			Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), m_texture.lock()->GetSRV().GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetGraphicsRootDescriptorTable(Ideal::RectRootSignature::Slot::SRV_Sprite, handle.GetGpuHandle());
		}

		// Sprite Constant Buffer
		{
			auto cb = CBPool->Allocate(Device.Get(), sizeof(CB_Sprite));
			memcpy(cb->SystemMemAddr, &m_cbSprite, sizeof(CB_Sprite));
			auto handle = UIDescriptorHeap->Allocate();
			Device->CopyDescriptorsSimple(1, handle.GetCpuHandle(), cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CommandList->SetGraphicsRootDescriptorTable(Ideal::RectRootSignature::Slot::CBV_RectInfo, handle.GetGpuHandle());
		}
	}
	// TODO : DrawIndexedInstanced
	CommandList->DrawIndexedInstanced(m_mesh.lock()->GetElementCount(), 1, 0, 0, 0);
}

void Ideal::IdealSprite::SetScreenPosition(const Vector2& ScreenPos)
{
	m_cbSprite.ScreenPos = ScreenPos;
}

void Ideal::IdealSprite::SetPosition(const Vector2& Position)
{
	m_cbSprite.Pos = Position;
}

void Ideal::IdealSprite::SetTextureSize(const Vector2& TextureSize)
{
	m_cbSprite.TexSize = TextureSize;
}

void Ideal::IdealSprite::SetTextureSamplePosition(const Vector2& TextureSamplePosition)
{
	m_cbSprite.TexSamplePos;
}

void Ideal::IdealSprite::SetTextureSampleSize(const Vector2& TextureSampleSize)
{
	m_cbSprite.TexSampleSize;
}

void Ideal::IdealSprite::SetZ(float Z)
{
	m_cbSprite.Z = Z;
}

void Ideal::IdealSprite::SetAlpha(float Alpha)
{
	m_cbSprite.Alpha = Alpha;
}

void Ideal::IdealSprite::SetTexture(std::weak_ptr<Ideal::ITexture> Texture)
{
	m_texture = std::static_pointer_cast<Ideal::D3D12Texture>(Texture.lock());
}

void Ideal::IdealSprite::SetMesh(std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> Mesh)
{
	m_mesh = Mesh;
}
