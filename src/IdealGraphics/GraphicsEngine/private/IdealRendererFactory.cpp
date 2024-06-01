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
		OutRenderer = std::make_shared<Ideal::D3D12Renderer>(*hwnd, Width, Height, false);
		std::static_pointer_cast<Ideal::D3D12Renderer>(OutRenderer)->SetAssetPath(AssetPath);
		std::static_pointer_cast<Ideal::D3D12Renderer>(OutRenderer)->SetModelPath(ModelPath);
		std::static_pointer_cast<Ideal::D3D12Renderer>(OutRenderer)->SetTexturePath(TexturePath);
	}
	else if (APIType == EGraphicsInterfaceType::D3D12_EDITOR)
	{
		OutRenderer = std::make_shared<Ideal::D3D12Renderer>(*hwnd, Width, Height, true);
		std::static_pointer_cast<Ideal::D3D12Renderer>(OutRenderer)->SetAssetPath(AssetPath);
		std::static_pointer_cast<Ideal::D3D12Renderer>(OutRenderer)->SetModelPath(ModelPath);
		std::static_pointer_cast<Ideal::D3D12Renderer>(OutRenderer)->SetTexturePath(TexturePath);
	}
	else
	{
		OutRenderer = nullptr;
	}

	return OutRenderer;
}
