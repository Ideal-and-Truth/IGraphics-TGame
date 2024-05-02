#include "StringConverter.h"
#include "Component.h"
#include "Entity.h"
template<>
std::string StringConverter::ToString(bool _val, int _indent)
{
	if (_val)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}

template<>
std::string StringConverter::ToString(Vector4 _val, int _indent)
{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += ", ";
	result += ToString(_val.z);
	result += ", ";
	result += ToString(_val.w);
	result += " }";
	return result;
}

template<>
std::string StringConverter::ToString(Vector3 _val, int _indent)
{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += ", ";
	result += ToString(_val.z);
	result += " }";
	return result;
}

template<>
std::string StringConverter::ToString(Vector2 _val, int _indent)
{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += " }";
	return result;
}

template<>
std::string StringConverter::ToString(std::shared_ptr<Component> _val, int _indent)
{
	std::string result = "";
	result += _val->GetTypeInfo().Dump(_val.get(), _indent);
	return result;
}

template<>
std::string StringConverter::ToString(std::shared_ptr<Entity> _val, int _indent)
{
	return "Entity";
}

template<>
std::string StringConverter::ToString(std::string _val, int _indent)
{
	return _val;
}
