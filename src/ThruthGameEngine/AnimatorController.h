#pragma once
#include "Component.h"

namespace Truth
{
	class SkinnedMesh;
}

namespace Truth
{

	struct AnimatorState
	{
		std::string nodeName = "";
		std::string animationName = "";
		float4 animationSpeed = 0.f;
		bool isDefaultState = false;
		bool isActivated = false;
		bool isLoopTime = false;
	};


	class AnimatorController :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(AnimatorController);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<AnimatorState> m_entry;
		std::shared_ptr<AnimatorState> m_exit;

		std::shared_ptr<Truth::Component> m_state;


	private:
		PROPERTY(currentStateName);
		std::string m_currentStateName;

		PROPERTY(currentState);
		std::shared_ptr<AnimatorState> m_currentState;
		PROPERTY(states);
		std::vector<std::shared_ptr<AnimatorState>> m_states;

		std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;

	public:
		AnimatorController();
		virtual ~AnimatorController();

		METHOD(Initalize);
		void Initalize();

		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();

		METHOD(Update);
		void Update();

	private:
		bool m_isAnimationEnd;
		void CheckTransition();
		void NextState();

	};
	template<class Archive>
	void Truth::AnimatorController::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}

	template<class Archive>
	void Truth::AnimatorController::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}

}

BOOST_CLASS_EXPORT_KEY(Truth::AnimatorController)
BOOST_CLASS_VERSION(Truth::AnimatorController, 0)