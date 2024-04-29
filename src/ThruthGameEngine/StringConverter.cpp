#include "StringConverter.h"

template<>
std::string StringConverter::ToString(bool _val)
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
std::string StringConverter::ToString(Vector4 _val)
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
std::string StringConverter::ToString(Vector3 _val)

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
std::string StringConverter::ToString(Vector2 _val)

{
	std::string result = "";
	result += "{ ";
	result += ToString(_val.x);
	result += ", ";
	result += ToString(_val.y);
	result += " }";
	return result;
}
