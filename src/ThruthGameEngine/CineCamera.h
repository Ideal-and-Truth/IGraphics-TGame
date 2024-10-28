#pragma once
#include "Component.h"

namespace Truth
{
	class CineCamera :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(CineCamera)
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);
	};
}

template<class Archive>
void Truth::CineCamera::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<Component>(*this);
}

template<class Archive>
void Truth::CineCamera::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}
BOOST_CLASS_EXPORT_KEY(Truth::CineCamera)
BOOST_CLASS_VERSION(Truth::CineCamera, 0)