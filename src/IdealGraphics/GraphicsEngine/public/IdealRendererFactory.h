#pragma once

#include <memory>
#include <string>

// ��� d3d12�ۿ� ����
enum class EGraphicsInterfaceType
{
	D3D12,
};

namespace Ideal
{
	class IdealRenderer;
}

// AssetPath : ���� ������
// ModelPath : Ŀ���� �������� ���� �ϰ� �� ���� ��ġ
// TexturePath: Ŀ���� ���� & Texture ���� �ϰ� �� ���� ��ġ
std::shared_ptr<Ideal::IdealRenderer> CreateRenderer(
	const EGraphicsInterfaceType& APIType,
	void* Hwnd,
	unsigned int Width,
	unsigned int Height,
	const std::wstring& AssetPath,
	const std::wstring& ModelPath,
	const std::wstring& TexturePath
	);
