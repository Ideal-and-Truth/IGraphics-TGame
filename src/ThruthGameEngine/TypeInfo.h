#pragma once

#include "types.h"

#include <concepts>
#include <map>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>

class Method;
class Property;
class TypeInfo;

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
/// Super 타입이 있는지
/// </summary>
template <typename , typename = void>
struct SuperClassTypeDeduction
{
	using Type = void;
};

template <typename T>
struct SuperClassTypeDeduction<T, std::void_t<typename T::ThisType>>
{
	using Type = T::ThisType;
};

template <typename T>
struct TypeInfoInitializer
{
	TypeInfoInitializer(const char* name)
		: m_name(name)
	{
		if constexpr (HasSuperType<T>)
		{
			m_super = &(typename T::Super::StaticTypeInfo());
		}
	}

	const char* m_name = nullptr;
	const TypeInfo* m_super = nullptr;
};

class TypeInfo
{
public:
	template <typename T>
	explicit TypeInfo(const TypeInfoInitializer<T>& initializer)
		: m_typeHash(typeid(T).hash_code())
		, m_name(initializer.m_name)
		, m_fullName(typeid(T).name())
		, m_super(initializer.m_super)
		, m_isArray(std::is_array_v<T>)
	{
		if constexpr (HasSuperType<T>)
		{
			CollectSuperMethods();
			CollectSuperProperties();
		}
	}

	const TypeInfo* GetSuper() const;

	template <typename T, std::enable_if_t<HasStaticTypeInfoFunc<T>::value>* = nullptr>
	static const TypeInfo& GetStaticTypeInfo()
	{
		return T::StaticTypeInfo();
	}

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

	bool IsA(const TypeInfo& other) const;
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

	const char* GetName() const
	{
		return m_name;
	}

	bool IsArray() const
	{
		return m_isArray;
	}

private:
	void CollectSuperMethods();
	void CollectSuperProperties();

	friend Method;
	friend Property;
	using MethodMap = std::map<std::string_view, const Method*>;
	using PropertyMap = std::map<std::string_view, const Property*>;

	void AddProperty(const Property* property);
	void AddMethod(const Method* method);

	size_t m_typeHash;
	const char* m_name = nullptr;
	std::string m_fullName;
	const TypeInfo* m_super = nullptr;

	bool m_isArray = false;

	std::vector<const Method*> m_methods;
	MethodMap m_methodMap;

	std::vector<const Property*> m_properties;
	PropertyMap m_propertyMap;
};