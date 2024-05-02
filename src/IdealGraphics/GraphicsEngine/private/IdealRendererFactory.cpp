#pragma once
#include "GraphicsEngine/public/IdealRendererFactory.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include <windows.h>

std::shared_ptr<Ideal::IdealRenderer> CreateRenderer(
	const EGraphicsInterfaceType& APIType,
	void* Hwnd, unsigned int Width, unsigned int Height,
	const std::wstring& AssetPath,
	const std::wstring& ModelPath,
	const std::wstring& TexturePath)
{
	std::shared_ptr<Ideal::IdealRenderer> OutRenderer = nullptr;

	HWND* hwnd = (HWND*)Hwnd;

	if (APIType == EGraphicsInterfaceType::D3D12)
	{
		OutRenderer = std::make_shared<D3D12Renderer>(*hwnd, Width, Height);
		std::static_pointer_cast<D3D12Renderer>(OutRenderer)->SetAssetPath(AssetPath);
		std::static_pointer_cast<D3D12Renderer>(OutRenderer)->SetModelPath(ModelPath);
		std::static_pointer_cast<D3D12Renderer>(OutRenderer)->SetTexturePath(TexturePath);
	}

	return OutRenderer;
}
