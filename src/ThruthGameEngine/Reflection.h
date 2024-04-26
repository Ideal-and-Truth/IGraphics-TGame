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
	/// 타입을 설명하는 구조체
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

	// 타입 설명 구조체를 반환하는 탬플릿 함수
	// 필요한 타입에 따라 생성해 주어야 한다.
	template <typename T>
	TypeDescriptor* GetPrimitiveDescriptor();
#pragma endregion TypeDescriptor

#pragma region DefaultResolver
	/// <summary>
	/// 기본 타입 해설자
	/// </summary>
	struct DefaultResolver
	{
		// 리플렉션이 가능한 객체인지 판별하는 함수
		// 함수의 반환 값이 중요할 뿐, 함수 자체는 아무 역할을 하지 않는다.
		template <typename T>
		static char Func(decltype(&T::Reflection));
		template <typename T>
		static int Func(...);

		// 해당 객체가 리플렉션 적용 가능한지 체크하는 구조체
		template <typename T>
		struct IsReflected
		{
			enum
			{
				// &T::Reflection이 있다면 char 사이즈 반환
				// 없다면 int 사이즈를 반환 이를 통해 value 에 bool 값이 저장된다.
				value = (sizeof(Func<T>(nullptr)) == sizeof(char))
			};
		};

		// 리플렉션 가능한 객체에 대한 객체 정보 가져오기 (바로 리플렉션 객체를 리턴한다.)
		template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return &T::Reflection;
		}

		// 리플렉션 불가능 객체에 대한 객체 정보 가져오기. (없다면 해당 타입에 대한 리플렉션을 리턴한다.)
		template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return GetPrimitiveDescriptor<T>();
		}
	};
#pragma endregion DefaultResolver

#pragma region TypeResolver
	/// <summary>
	/// 타입 해설자
	/// </summary>
	/// <typeparam name="T">타입</typeparam>
	template <typename T>
	struct TypeResolver
	{
		// 기본 해설자를 통해 객체의 정보 가져옴 
		static TypeDescriptor* Get()
		{
			return DefaultResolver::Get<T>();
		}
	};
#pragma endregion TypeResolver

#pragma region TypeDescriptor_Struct
	/// <summary>
	/// 구조체 (클래스)에 대한 타입 설명자
	/// 코딩 컨벤션에 맞지는 않지만 가독성을 위해 설명자에 한해 _ 으로 타입을 분리
	/// </summary>
	struct TypeDescriptor_Struct
		: TypeDescriptor
	{
		// 멤버 변수들
		struct Member
		{
			const char* m_name;
			size_t m_offset;
			TypeDescriptor* m_type;
		};
		// 멤버 변수가 여러개이므로
		std::vector<Member> m_members;

		// initalize 함수를 받아와서 사용
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

		// 덤프 함수
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
	/// std::vector 에 대한 타입 설명자
	/// </summary>
	struct TypeDescriptor_StdVector
		: TypeDescriptor
	{
		// vector 타입에 대한 설명자
		TypeDescriptor* m_itemType;

		// 사이즈 리턴 함수 (람다)
		size_t(*GetSize)(const void*);
		// vector 안에 값 리턴 함수 (람다)
		const void* (*GetItem)(const void*, size_t);

		// 생성자
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
		// map 타입에 대한 설명자
		TypeDescriptor* m_keyType;
		TypeDescriptor* m_valueType;

		// 사이즈 리턴 함수 (람다)
		size_t(*GetSize)(const void*);
		std::string(*GetItem)(const void*, int);

		// 생성자
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