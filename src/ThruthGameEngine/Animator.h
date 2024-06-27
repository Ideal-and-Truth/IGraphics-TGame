#pragma once
#include "Component.h"


namespace Truth
{
	class AnimatorController;
	class SkinnedMesh;
}

namespace Truth
{
	
	class Animator :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Animator);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Truth::AnimatorController> m_animatorController;
		std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;

	public:
		Animator();
		virtual ~Animator();

		METHOD(Initalize);
		void Initalize();

		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();

		METHOD(Update);
		void Update();


	};

	template<class Archive>
	void Truth::Animator::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}

	template<class Archive>
	void Truth::Animator::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}
}



BOOST_CLASS_EXPORT_KEY(Truth::Animator)
BOOST_CLASS_VERSION(Truth::Animator, 0)