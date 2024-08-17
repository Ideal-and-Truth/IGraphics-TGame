#pragma once
#include "Isprite.h"
#include <string>

namespace Ideal
{
	class IText : public ISprite
	{
	public:
		IText() {}
		virtual ~IText() {}

	public:
		virtual void ChangeText(const std::wstring&) abstract;

	};
}