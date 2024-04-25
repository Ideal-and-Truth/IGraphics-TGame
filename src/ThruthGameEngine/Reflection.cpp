#include "Reflection.h"

namespace reflect {
#pragma region INT
	struct TypeDescriptor_Int 
		: TypeDescriptor 
	{
		TypeDescriptor_Int() 
			: TypeDescriptor{ "int", sizeof(int) } 
		{
		}
		virtual std::string Dump(const void* obj, int _indent) const override
		{
			return "int{" + std::to_string(*(const int*)obj) + "}";
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<int>() 
	{
		static TypeDescriptor_Int typeDesc;
		return &typeDesc;
	}
#pragma endregion INT

#pragma region SIZE_T
	struct TypeDescriptor_Size_t
		: TypeDescriptor
	{
		TypeDescriptor_Size_t()
			: TypeDescriptor{ "size_t", sizeof(size_t) }
		{
		}
		virtual std::string Dump(const void* obj, int _indent) const override
		{
			return "size_t{" + std::to_string(*(const size_t*)obj) + "}";
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<size_t>()
	{
		static TypeDescriptor_Size_t typeDesc;
		return &typeDesc;
	}
#pragma endregion SIZE_T

#pragma region USHORT
	struct TypeDescriptor_UnsignedShort
		: TypeDescriptor
	{
		TypeDescriptor_UnsignedShort()
			: TypeDescriptor{ "unsigned short", sizeof(unsigned short) }
		{
		}
		virtual std::string Dump(const void* obj, int _indent) const override
		{
			return "unsigned short{" + std::to_string(*(const unsigned short*)obj) + "}";
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<unsigned short>()
	{
		static TypeDescriptor_UnsignedShort typeDesc;
		return &typeDesc;
	}
#pragma endregion USHORT

#pragma region STRING
	struct TypeDescriptor_StdString 
		: TypeDescriptor 
	{
		TypeDescriptor_StdString() 
			: TypeDescriptor{ "std::string", sizeof(std::string) } 
		{
		}
		virtual std::string Dump(const void* obj, int _indent) const override
		{
			return "std::string{\"" + *(const std::string*)obj + "\"}";
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<std::string>() 
	{
		static TypeDescriptor_StdString typeDesc;
		return &typeDesc;
	}
#pragma endregion STRING
}