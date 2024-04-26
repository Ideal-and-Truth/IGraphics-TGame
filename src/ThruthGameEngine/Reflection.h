#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <cstddef>
#include <map>
#include <functional>

namespace reflect
{
#pragma region TypeDescriptor
	/// <summary>
	/// Ÿ���� �����ϴ� ����ü
	/// </summary>
	struct TypeDescriptor
	{
		std::string m_name;
		size_t m_size;

		TypeDescriptor(std::string _name, size_t _size)
			: m_name{ _name }
			, m_size{ _size }
		{
		}

		virtual ~TypeDescriptor() {}
		virtual std::string GetFullName() const { return m_name; }
		virtual std::string Dump(const void* _obj, int _indentLevel = 0) const abstract;
	};

	// Ÿ�� ���� ����ü�� ��ȯ�ϴ� ���ø� �Լ�
	// �ʿ��� Ÿ�Կ� ���� ������ �־�� �Ѵ�.
	template <typename T>
	TypeDescriptor* GetPrimitiveDescriptor();
#pragma endregion TypeDescriptor

#pragma region DefaultResolver
	/// <summary>
	/// �⺻ Ÿ�� �ؼ���
	/// </summary>
	struct DefaultResolver
	{
		// ���÷����� ������ ��ü���� �Ǻ��ϴ� �Լ�
		// �Լ��� ��ȯ ���� �߿��� ��, �Լ� ��ü�� �ƹ� ������ ���� �ʴ´�.
		template <typename T>
		static char Func(decltype(&T::Reflection));
		template <typename T>
		static int Func(...);

		// �ش� ��ü�� ���÷��� ���� �������� üũ�ϴ� ����ü
		template <typename T>
		struct IsReflected
		{
			enum
			{
				// &T::Reflection�� �ִٸ� char ������ ��ȯ
				// ���ٸ� int ����� ��ȯ �̸� ���� value �� bool ���� ����ȴ�.
				value = (sizeof(Func<T>(nullptr)) == sizeof(char))
			};
		};

		// ���÷��� ������ ��ü�� ���� ��ü ���� �������� (�ٷ� ���÷��� ��ü�� �����Ѵ�.)
		template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return &T::Reflection;
		}

		// ���÷��� �Ұ��� ��ü�� ���� ��ü ���� ��������. (���ٸ� �ش� Ÿ�Կ� ���� ���÷����� �����Ѵ�.)
		template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return GetPrimitiveDescriptor<T>();
		}
	};
#pragma endregion DefaultResolver

#pragma region TypeResolver
	/// <summary>
	/// Ÿ�� �ؼ���
	/// </summary>
	/// <typeparam name="T">Ÿ��</typeparam>
	template <typename T>
	struct TypeResolver
	{
		// �⺻ �ؼ��ڸ� ���� ��ü�� ���� ������ 
		static TypeDescriptor* Get()
		{
			return DefaultResolver::Get<T>();
		}
	};
#pragma endregion TypeResolver

#pragma region TypeDescriptor_Struct
	/// <summary>
	/// ����ü (Ŭ����)�� ���� Ÿ�� ������
	/// �ڵ� �����ǿ� ������ ������ �������� ���� �����ڿ� ���� _ ���� Ÿ���� �и�
	/// </summary>
	struct TypeDescriptor_Struct
		: TypeDescriptor
	{
		// ��� ������
		struct Member
		{
			const char* m_name;
			size_t m_offset;
			TypeDescriptor* m_type;
		};
		// ��� ������ �������̹Ƿ�
		std::vector<Member> m_members;

		// initalize �Լ��� �޾ƿͼ� ���
		TypeDescriptor_Struct(void (*_init)(TypeDescriptor_Struct*))
			: TypeDescriptor{ "", 0 }
		{
			_init(this);
		}

		TypeDescriptor_Struct(std::string _name, size_t _size, const std::initializer_list<Member>& _init)
			: TypeDescriptor{ "", 0 }
			, m_members{ _init }
		{
		}

		// ���� �Լ�
		virtual std::string Dump(const void* _obj, int _indent) const override
		{
			std::string result = "";
			result += std::string((_indent), '\t') + m_name + "\n" + std::string((_indent), '\t') + "{\n";

			for (const Member& member : m_members)
			{
				result += std::string((_indent + 1), '\t') + member.m_name + " = ";
				result += member.m_type->Dump((char*)_obj + member.m_offset, _indent + 1);
				result += "\n";
			}
			result += std::string(_indent, '\t') + "}\n";
			return result;
		}
	};
#pragma endregion TypeDescriptor_Struct

#pragma region REFLECT
#define REFLECT() \
    friend struct reflect::DefaultResolver; \
    static reflect::TypeDescriptor_Struct Reflection; \
    static void InitReflection(reflect::TypeDescriptor_Struct*);

#define REFLECT_STRUCT_BEGIN(type) \
    reflect::TypeDescriptor_Struct type::Reflection{ type::InitReflection }; \
    void type::InitReflection(reflect::TypeDescriptor_Struct* typeDesc) { \
        using T = type; \
        typeDesc->m_name = #type; \
        typeDesc->m_size = sizeof(T); \
        typeDesc->m_members = {

#define REFLECT_STRUCT_MEMBER(name) \
            {#name, offsetof(T, name), reflect::TypeResolver<decltype(T::name)>::Get()},

#define REFLECT_STRUCT_END() \
        }; \
    }
#pragma endregion REFLECT

#pragma region VECTOR
	/// <summary>
	/// std::vector �� ���� Ÿ�� ������
	/// </summary>
	struct TypeDescriptor_StdVector
		: TypeDescriptor
	{
		// vector Ÿ�Կ� ���� ������
		TypeDescriptor* m_itemType;

		// ������ ���� �Լ� (����)
		size_t(*GetSize)(const void*);
		// vector �ȿ� �� ���� �Լ� (����)
		const void* (*GetItem)(const void*, size_t);

		// ������
		template <typename ItemType>
		TypeDescriptor_StdVector(ItemType*)
			: TypeDescriptor{ "std::vector<>", sizeof(std::vector<ItemType>) }
			, m_itemType{ TypeResolver<ItemType>::Get() }
		{
			GetSize = [](const void* vecPtr) -> size_t
				{
					const auto& vec = *(const std::vector<ItemType>*) vecPtr;
					return vec.size();
				};
			GetItem = [](const void* vecPtr, size_t index) -> const void*
				{
					const auto& vec = *(const std::vector<ItemType>*) vecPtr;
					return &vec[index];
				};
		}

		virtual std::string GetFullName() const override
		{
			return std::string("std::vector<") + m_itemType->GetFullName() + ">";
		}

		virtual std::string Dump(const void* obj, int indentLevel) const override
		{
			size_t numItems = GetSize(obj);
			std::string result = "";
			result += GetFullName();
			if (numItems == 0)
			{
				result += "{}";
			}
			else
			{
				result += "\n" + std::string(indentLevel, '\t') + "{\n";
				for (size_t index = 0; index < numItems; index++)
				{
					result += std::string((indentLevel + 1), '\t') + "[" + std::to_string(index) + "] ";
					result += m_itemType->Dump(GetItem(obj, index), indentLevel + 1);
					result += "\n";
				}
				result += std::string(indentLevel, '\t') + "}";
			}

			return result;
		}
	};

	template <typename T>
	class TypeResolver<std::vector<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdVector typeDesc{ (T*) nullptr };
			return &typeDesc;
		}
	};
#pragma endregion VECTOR

#pragma region SHARED_PTR

	struct TypeDescriptor_SharedPtr
		: TypeDescriptor
	{
		TypeDescriptor* m_itemType;

		template<typename T>
		TypeDescriptor_SharedPtr(T*)
			: TypeDescriptor{ "std::shared_ptr<>", sizeof(std::shared_ptr<T>) }
			, m_itemType(TypeResolver<T>::Get())
		{
		}

		virtual std::string GetFullName() const override
		{
			return std::string(m_itemType->GetFullName());
		}

		virtual std::string Dump(const void* obj, int _indent) const override
		{
			std::string result = "";
			result += GetFullName();
			result += "\n" + std::string((_indent), '\t') + "{\n";
			result += m_itemType->Dump(obj, _indent + 1);
			result += std::string(_indent, '\t') + "}";

			return result;
		}
	};

	template <typename T>
	class TypeResolver<std::shared_ptr<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_SharedPtr typeDesc{ (T*) nullptr };
			return &typeDesc;
		}
	};
#pragma endregion SHARED_PTR

#pragma region MAP
	struct TypeDescriptor_StdMap
		: TypeDescriptor
	{
		// map Ÿ�Կ� ���� ������
		TypeDescriptor* m_keyType;
		TypeDescriptor* m_valueType;

		// ������ ���� �Լ� (����)
		size_t(*GetSize)(const void*);
		std::string(*GetItem)(const void*, int);

		// ������
		template <typename KeyType, typename ValueType>
		TypeDescriptor_StdMap(KeyType*, ValueType*)
			: TypeDescriptor{ "std::map<>", sizeof(std::map<KeyType, ValueType>) }
			, m_keyType{ TypeResolver<KeyType>::Get() }
			, m_valueType{ TypeResolver<ValueType>::Get() }
		{
			GetSize = [](const void* _mapPtr) -> size_t
				{
					const auto& mapData = *(const std::map<KeyType, ValueType>*) _mapPtr;
					return mapData.size();
				};
			GetItem = [](const void* _mapPtr, int _indent) -> std::string
				{
					std::string result = "";
					const auto& mapData = *(const std::map<KeyType, ValueType>*) _mapPtr;
					for (const auto& p : mapData)
					{
						result += std::string(_indent, '\t');
						result += "[" + TypeResolver<KeyType>::Get()->Dump(&(p.first), _indent + 1) + "] : ";
						result += TypeResolver<ValueType>::Get()->Dump(&(p.second), _indent + 1);
						result += "\n";
					}
					return result;
				};
		}

		virtual std::string GetFullName() const override
		{
			return std::string("std::map<") + m_keyType->GetFullName() + ", " + m_valueType->GetFullName() + ">";
		}

		virtual std::string Dump(const void* obj, int _indent) const override
		{
			size_t numItems = GetSize(obj);
			std::string result = "";
			result += GetFullName();
			if (numItems == 0)
			{
				result += "{}";
			}
			else
			{
				result += "\n" + std::string(_indent, '\t') + "{\n";
				result += GetItem(obj, _indent + 1);
				result += std::string(_indent, '\t') + "}";
			}
			return result;
		}
	};

	template <typename T, typename U>
	class TypeResolver<std::map<T, U>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdMap typeDesc((T*) nullptr, (U*) nullptr);
			return &typeDesc;
		}
	};
#pragma endregion MAP
}