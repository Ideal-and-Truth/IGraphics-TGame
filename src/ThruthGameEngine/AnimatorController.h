#pragma once
#include "Component.h"

namespace Truth
{
	struct AnimatorTransition;
	struct AnimatorState
	{
		std::string nodeName = "";
		std::string animationName = "";
		float4 animationSpeed = 0.f;
		bool isDefaultState = false;
		bool isActivated = false;
		bool isLoopTime = false;
		std::vector<Truth::AnimatorTransition> nextAnimatorState;

	};

	struct AnimatorTransition
	{
		Truth::AnimatorState nextAnimatorState;
		bool condition = false;
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

	protected:
		AnimatorState m_entry
		{
			m_entry.nodeName = "Entry",
			m_entry.animationName = "",
			m_entry.animationSpeed = 0.f,
			m_entry.isDefaultState = false,
			m_entry.isActivated = false,
			m_entry.isLoopTime = false
		};

		AnimatorState m_exit
		{
			m_exit.nodeName = "Exit",
			m_exit.animationName = "",
			m_exit.animationSpeed = 0.f,
			m_exit.isDefaultState = false,
			m_exit.isActivated = false,
			m_exit.isLoopTime = false
		};

		AnimatorState m_currentState;
		std::vector<AnimatorState> m_states;

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