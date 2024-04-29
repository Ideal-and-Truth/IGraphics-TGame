#pragma once
#include <string>
#include <type_traits>
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

/// <summary>
/// ���ø� Ư��ȭ�� ���� �پ��� Ÿ���� ���ڿ�ȭ
/// �ʿ��� Ÿ���� �ִٸ� ���� �����Ͽ��� ��
/// </summary>
namespace StringConverter
{
	// int, float �� ���� �Ǽ� ���� �̰� ����Ѵ�
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
	}

	// �׿� ������ Ÿ���� �Ʒ��� ���� �����ؼ� ����Ͽ����Ѵ�.

	template<>
	std::string ToString(bool _val);


	template<>
	std::string ToString(Vector4 _val);

	template<>
	std::string ToString(Vector3 _val);

	template<>
	std::string ToString(Vector2 _val);

}



