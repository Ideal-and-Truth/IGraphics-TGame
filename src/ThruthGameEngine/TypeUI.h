#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include "SimpleMath.h"
#include "imgui.h"

using namespace DirectX::SimpleMath;

// template <typename, typename = void>
// struct IsStdVector
// 	: std::false_type
// {
// };
// template <typename T, typename Alloc>
// struct IsStdVector<std::vector<T, Alloc>>
// 	: std::true_type
// {
// };
// 
// template <typename, typename = void>
// struct IsStdList
// 	: std::false_type
// {
// };
// template <typename T, typename Alloc>
// struct IsStdList<std::list<T, Alloc>>
// 	: std::true_type
// {
// };
// 
// template <typename, typename = void>
// struct IsStdMap
// 	: std::false_type
// {
// };
// template<typename T>
// struct IsStdMap<T, std::void_t<typename T::key_type,
// 	typename T::mapped_type,
// 	decltype(std::declval<T&>()[std::declval<const typename T::key_type&>()])>>
// 	: std::true_type { };
// 
// template <typename = void>
// struct IsStdSharedPtr
// 	: std::false_type
// {
// };
// template <typename T>
// struct IsStdSharedPtr<std::shared_ptr<T>>
// 	: std::true_type
// {
// };

namespace Truth
{
	class Entity;
	class Component;
	class Scene;
}

namespace TypeUI
{
	// 템플릿 타입에 대한 타입은 여기서 정의한다.
	template<typename T>
	bool DisplayType(T& _val, const char* _name)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			return ImGui::DragInt(_name, &_val);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return ImGui::Checkbox(_name, &_val);
		}
		else if constexpr (std::is_same_v<T, unsigned int>)
		{
			return ImGui::DragScalar(_name, ImGuiDataType_::ImGuiDataType_U64, &_val);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return ImGui::DragFloat(_name, &_val);
		}
		else if constexpr (std::is_same_v<T, Vector3>)
		{
			float value[3] = {};
			value[0] = _val.x;
			value[1] = _val.y;
			value[2] = _val.z;
			bool isSelect = ImGui::DragFloat3(_name, value, 0.01f);
			if (isSelect)
			{
				_val.x = value[0];
				_val.y = value[1];
				_val.z = value[2];
			}
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, Vector2>)
		{
			float value[3] = {};
			value[0] = _val.x;
			value[1] = _val.y;
			bool isSelect = ImGui::DragFloat3(_name, value, 0.01f);
			if (isSelect)
			{
				_val.x = value[0];
				_val.y = value[1];
			}
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			char* cMeshPath = (char*)_val.c_str();
			return ImGui::InputText(_name, cMeshPath, 128);
		}
		else if constexpr (std::is_same_v<T, Quaternion>)
		{
			float value[3] = {};
			Vector3 temp = _val.ToEuler();
			value[0] = temp.x;
			value[1] = temp.y;
			value[2] = temp.z;
			bool isSelect = ImGui::DragFloat3(_name, value, 0.01f);
			if (isSelect)
			{
				temp.x = value[0];
				temp.y = value[1];
				temp.z = value[2];
			}
			_val = Quaternion::CreateFromYawPitchRoll(temp);
			return isSelect;
		}

		return false;
	}
};

