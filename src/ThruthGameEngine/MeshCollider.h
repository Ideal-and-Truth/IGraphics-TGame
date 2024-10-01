#pragma once
#include "Collider.h"
namespace Truth
{
	class MeshCollider 
		: public Collider
	{
		GENERATE_CLASS_TYPE_INFO(MeshCollider);

		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& _ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& _ar, const unsigned int file_version);

	private:
		std::vector<std::vector<Vector3>> m_points;
		std::wstring m_path;
		std::vector<physx::PxShape*> m_meshCollider;

	public:
		MeshCollider();
		MeshCollider(std::string _path);
		~MeshCollider();

		METHOD(Initalize);
		void Initalize();

		METHOD(Awake);
		virtual void Awake() override;

	private:
		void GetPoints();

	};

	template<class Archive>
	void Truth::MeshCollider::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Collider>(*this);
	}

	template<class Archive>
	void Truth::MeshCollider::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Collider>(*this);
	}

}

BOOST_CLASS_EXPORT_KEY(Truth::MeshCollider)
BOOST_CLASS_VERSION(Truth::MeshCollider, 0)