#pragma once
#include <map>
#include <string>
#include <vector>
#include <typeinfo>
#include <memory>

class Method;
class Property;
class TypeInfo;

template <typename T>
struct IsVectorContainer {
	static const bool value = false;
};
template <typename T, typename Alloc>
struct IsVectorContainer<std::vector<T, Alloc> > {
	static const bool value = true;
};

/// <summary>
/// Super 타입이 있는지
/// </summary>
template<typename, typename = void>
struct HasSuperType
	: std::false_type
{
};
template<typename T>
struct HasSuperType<T, std::void_t<typename T::Super>>
	: std::true_type
{
};

/// <summary>
/// StaticTypeInfo 함수가 있는지
/// </summary>
template<typename, typename = void>
struct HasStaticTypeInfoFunc
	: std::false_type
{
};
template<typename T>
struct HasStaticTypeInfoFunc<T, std::void_t<decltype(std::declval<T>().HasStaticTypeInfoFunc())>>
	: std::true_type
{
};

/// <summary>
/// ThisType이 타입이 있는지
/// </summary>
template <typename, typename = void>
struct SuperClassTypeDeduction
{
	using Type = void;
};
template <typename T>
struct SuperClassTypeDeduction<T, std::void_t<typename T::ThisType>>
{
	using Type = typename T::ThisType;
};

/// <summary>
/// 타입 정보 초기화 구조체
/// </summary>
/// <typeparam name="T">타입</typeparam>
template <typename T>
struct TypeInfoInitializer
{
	// 이름
	const char* m_name = nullptr;
	// 부모 클래스
	const TypeInfo* m_super = nullptr;

	// 이름은 생성자에서 초기화
	TypeInfoInitializer(const char* _name)
		: m_name(_name)
	{
		// 만일 super가 있다면
		if constexpr (HasSuperType<T>::value)
		{
			// 만일 super가 void가 아니라면
			// 즉 부모 클래스가 있다면
			using Type = typename T::Super;
			if constexpr (!std::is_void_v<Type>)
			{
				// 부모 클래스를가져온다.
				m_super = &(Type::StaticTypeInfo());
			}
		}
	}
};

/// <summary>
/// 타입 정보
/// </summary>
class TypeInfo
{
private:
	// 함수 정보
	friend Method;
	// 변수 정보
	friend Property;
	// string을 통한 맵으로 접근하기 위함
	using MethodMap = std::map<std::string_view, const Method*>;
	using PropertyMap = std::map<std::string_view, const Property*>;

	// 타임 이름 해시값
	size_t m_typeHash;
	// 이름
	const char* m_name = nullptr;
	// 타임의 전체 이름 (typeid.name())
	std::string m_fullName;
	// 부모 클래스의 정보 (있다면)
	const TypeInfo* m_super = nullptr;
	// 해당 타입이 배열인지
	bool m_isArray = false;

	// 함수 컨테이너 
	std::vector<const Method*> m_methods;
	MethodMap m_methodMap;

	// 변수 컨테이너
	std::vector<const Property*> m_properties;
	PropertyMap m_propertyMap;

public:
	std::string Dump(void* _object, int _indent = 0) const;

	// 생성자 
	// 타입 정보 이니셜라이저에서 필요한 정보를 받아온다.
	// 컴파일 시간 안에 일어나는 과정
	template <typename T>
	explicit TypeInfo(const TypeInfoInitializer<T>& initializer)
		: m_typeHash(typeid(T).hash_code())
		, m_name(initializer.m_name)
		, m_fullName(typeid(T).name())
		, m_super(initializer.m_super)
		, m_isArray(std::is_array_v<T>)
	{
		// 만일 부모 타입이 존재하는 경우
		// 매크로 함수로 정의되 있는 경우
		if constexpr (HasSuperType<T>::value)
		{
			using Type = typename T::Super;
			// 해당 부모 타입이 void가 아닌 경우
			if constexpr (!std::is_void_v<Type>)
			{
				CollectSuperMethods();
				CollectSuperProperties();
			}
		}
	}

	// 부모의 타입 정보 가져오기
	const TypeInfo* GetSuper() const;

	// 정적 변수로 타입 정보가 있다면 가져온다.
	template <typename T, std::enable_if_t<HasStaticTypeInfoFunc<T>::value>* = nullptr>
	static const TypeInfo& GetStaticTypeInfo()
	{
		return T::StaticTypeInfo();
	}

	// 해당 타입이 포인터라면 포인터를 제거하고 가져온다.
	template
		<
		typename T,
		std::enable_if_t<std::is_pointer_v<T>>* = nullptr,
		std::enable_if_t<HasStaticTypeInfoFunc<std::remove_pointer_t<T>>::value>* = nullptr
		>
		static const TypeInfo& GetStaticTypeInfo()
	{
		return std::remove_pointer_t<T>::StaticTypeInfo();
	}

	// 없다면 리플렉션 변수 타입이 아닌것
	template
		<
		typename T,
		std::enable_if_t<!HasStaticTypeInfoFunc<T>::value>* = nullptr,
		std::enable_if_t<!HasStaticTypeInfoFunc<std::remove_pointer_t<T>>::value>* = nullptr
		>
		static const TypeInfo& GetStaticTypeInfo()
	{
		static TypeInfo typeInfo{ TypeInfoInitializer<T>("unreflected_type_variable") };
		return typeInfo;
	}

	// ohter과 같은 타입인지
	bool IsA(const TypeInfo& other) const;
	// other의 자식인지
	bool IsChildOf(const TypeInfo& other) const;

	template <typename T>
	bool IsA() const
	{
		return IsA(GetStaticTypeInfo<T>());
	}

	template <typename T>
	bool IsChildOf() const
	{
		return IsChildOf(GetStaticTypeInfo<T>());
	}

	const std::vector<const Method*>& GetMethods() const
	{
		return m_methods;
	}

	const Method* GetMethod(const char* name) const;

	const std::vector<const Property*>& GetProperties() const
	{
		return m_properties;
	}

	const Property* GetProperty(const char* name) const;

	const char* GetName() const { return m_name; }
	std::string GetFullName() const { return m_fullName; }

	bool IsArray() const { return m_isArray; }

private:
	void CollectSuperMethods();
	void CollectSuperProperties();

	void AddProperty(const Property* property);
	void AddMethod(const Method* method);
};