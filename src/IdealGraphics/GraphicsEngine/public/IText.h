#pragma once
#include <string>

namespace Ideal
{
	class IText
	{
	public:
		IText() {}
		virtual ~IText() {}

	public:
		virtual void ChangeText(const std::wstring&) abstract;
		// TODO: Sprite Change
	};
}