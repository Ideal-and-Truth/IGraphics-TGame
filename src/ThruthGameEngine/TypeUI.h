#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include "SimpleMath.h"
#include "imgui.h"
#include <atlconv.h>
#include "Material.h"
#include "Texture.h"
#include "ITexture.h"
#include "IMaterial.h"

#define PI 3.1415926
#define RadToDeg 57.29577951f
#define DegToRad 0.017453293f

using namespace DirectX::SimpleMath;

namespace Truth
{
	class Entity;
	class Component;
	class Scene;
}
#pragma warning(disable : 6387)
#pragma warning(disable : 6255)
namespace TypeUI
{
	// 템플릿 타입에 대한 타입은 여기서 정의한다.
	template<typename T>
	bool DisplayType(T& _val, const char* _name, float _min = 0.0f, float _max = 0.0f)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			return ImGui::DragInt(_name, &_val, 1.0f, (int)_min, (int)_max);
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
			return ImGui::DragFloat(_name, &_val, 0.01f, _min, _max);
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
			return ImGui::InputText(_name, cMeshPath, 128, ImGuiInputTextFlags_EnterReturnsTrue);
		}
		else if constexpr (std::is_same_v<T, std::wstring>)
		{
			USES_CONVERSION;
			std::string sval(W2A(_val.c_str()));

			sval.resize(128);

			bool success = ImGui::InputText(_name, (char*)sval.c_str(), 128, ImGuiInputTextFlags_EnterReturnsTrue);
			if (success)
			{
				_val = A2W(sval.c_str());
			}
			return success;
		}
		else if constexpr (std::is_same_v<T, Quaternion>)
		{
			float value[3] = {};

			Vector3 delta;

			Vector3 temp = _val.ToEuler();

			temp.x = temp.x * RadToDeg;
			temp.y = temp.y * RadToDeg;
			temp.z = temp.z * RadToDeg;

			value[0] = temp.x;
			value[1] = temp.y;
			value[2] = temp.z;

			bool isSelect = ImGui::DragFloat3(_name, value, 0.1f);
			if (isSelect)
			{
				delta = {
					temp.x - value[0],
					temp.y - value[1],
					temp.z - value[2]
				};
			}

			delta.x = delta.x * DegToRad;
			delta.y = delta.y * DegToRad;
			delta.z = delta.z * DegToRad;

			_val = _val * Quaternion::CreateFromYawPitchRoll(delta);
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, Color>)
		{
			float value[4] = {};

			value[0] = _val.x;
			value[1] = _val.y;
			value[2] = _val.z;
			value[3] = _val.w;

			bool isSelect = ImGui::DragFloat4(_name, value, 0.001f, 0.0f, 1.0f);
			if (isSelect)
			{
				_val = {
					value[0],
					value[1],
					value[2],
					value[3]
				};
			}
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<Ideal::IMaterial>>>)
		{
			const ImVec2 size(100, 100);
			ImGui::Text("Texture");
			for (auto& mat : _val)
			{
				if (!mat->GetBaseMap().expired())
				{
					ImGui::ImageButton((ImTextureID)(mat->GetBaseMap().lock()->GetImageID()), size);
					ImGui::SameLine();
				}
				if (!mat->GetNomralMap().expired())
				{

					ImGui::ImageButton((ImTextureID)(mat->GetNomralMap().lock()->GetImageID()), size);
					ImGui::SameLine();
				}
				if (!mat->GetMaskMap().expired())
				{
					ImGui::ImageButton((ImTextureID)(mat->GetMaskMap().lock()->GetImageID()), size);
				}
			}
			return false;
		}
		return false;
	}
};

