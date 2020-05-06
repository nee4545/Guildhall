#pragma once

#include <string>
#include <vector>
#include <map>
#include "Engine/Core/XmlUtils.hpp"


class TypedPropertyBase
{
public:
	virtual ~TypedPropertyBase() {}
	virtual void const* GetUniqueID() = 0;

	template <typename T>
	bool Is();
};


template<typename T>
class TypedProperty : public TypedPropertyBase
{
public:

	T m_value;
	void const* GetUniqueID() override { return StaticUniqueID(); }

	static void const* const StaticUniqueID()
	{
		static int s_local = 0;
		return &s_local;
	}

};

//template <typename T>
//class TypedProperty<T*> : public TypedPropertyBase
//{
//public:
//	
//	void const* GetUniqueID() override { return StaticUniqueID(); }
//
//	static void const* const StaticUniqueID()
//	{
//		static int s_local = 0;
//		return &s_local;
//	}
//public:
//	T* m_value;
//};


template <typename T>
bool TypedPropertyBase::Is()
{
	return GetUniqueID() == TypedProperty<T>::StaticUniqueID();
}

//template<typename T>
//bool TypedPropertyBase::IsPointer()
//{
//	return GetUniqueID() == TypedProperty<T*>::StaticUniqueID();
//}

class NamedProperties
{
public:
	void PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element );


	template <typename T>
	void SetValue( std::string const& keyName , T const& value )
	{
		TypedPropertyBase* base = FindInMap( keyName );

		if ( base == nullptr )
		{
			TypedProperty<T>* prop = new TypedProperty<T>();
			prop->m_value = value;

			m_keyValuePairs[ keyName ] = prop;
		}
		else
		{
			delete base;

			TypedProperty<T>* prop = new TypedProperty<T>();
			prop->m_value = value;

			m_keyValuePairs[ keyName ] = prop;
		}

	};

	/*template<typename T>
	void SetValue(std::string const&keyName,T* const& value )
	{
		TypedPropertyBase* base = FindInMap( keyName );
		if(base== nullptr )
		{
			TypedProperty<T*> prop = TypedProperty<T*>();
			prop.m_value = value;
			m_keyValuePairs[ keyName ] = &prop;
		}
		else
		{
			delete base;

			TypedProperty<T*> prop = TypedProperty<T*>();
			prop.m_value = value;
			m_keyValuePairs[ keyName ] = &prop;
		}
	}*/

	/*template<typename T>
	T* GetValue(std::string const& keyName, T* const defValue)
	{
		TypedPropertyBase* base = FindInMap( keyName );
		if ( base != nullptr )
		{
			if(base->IsPointer<T*>() )
			{
				TypedProperty<T*>* prop = ( TypedProperty<T>* )base;
				return prop->m_value;
			}
		}
		return defValue;
	}*/

	template<typename T>
	T GetValue( std::string const& keyName , T const& defValue ) const
	{
		TypedPropertyBase* base = FindInMap( keyName );

		if ( base != nullptr )
		{
			if ( base->Is<T>() )
			{
				TypedProperty<T>* prop = ( TypedProperty<T>* )base;
				return prop->m_value;
			}
		}

		return defValue;
	};

	void SetValue( std::string const& keyname , char const* val )
	{
		SetValue<std::string>( keyname , val );
	}

	std::string GetValue( std::string const& keyName , char const* val ) const
	{
		return GetValue<std::string>( keyName , val );
	}

private:
	std::map<std::string , TypedPropertyBase*> m_keyValuePairs;

	TypedPropertyBase* FindInMap( std::string const& key ) const
	{
		auto iter = m_keyValuePairs.find( key );
		if ( iter != m_keyValuePairs.end() )
		{
			return iter->second;
		}
		else
		{
			return nullptr;
		}
	}

};