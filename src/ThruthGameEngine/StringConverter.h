#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// 템플릿 특수화를 통한 다양한 타입의 문자열화
/// 필요한 타입이 있다면 직접 수정하여도 됨
/// </summary>
namespace StringConverter
{
	// int, float 등 정수 실수 형만 이걸 사용한다
	template<typename T>
	std::string ToString(T _val)
	{
		if constexpr (std::is_integral_v<T>)
		{
			return std::to_string(_val);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return std::to_string(_val);
		}
		return "shared ptr";

	}

	template<typename T>
	std::string ToString(std::shared_ptr<T> _val)
	{
		return "shared ptr";
	}

	// 그외 나머지 타입은 아래에 따로 정의해서 사용하여야한다.

	template<>
	std::string ToString(bool _val);

	template<>
	std::string ToString(Vector4 _val);

	template<>
	std::string ToString(Vector3 _val);

	template<>
	std::string ToString(Vector2 _val);

}



