#pragma once
#include "Component.h"
#include "GraphEditor.h"

namespace Truth
{
	class SkinnedMesh;
}

namespace Truth
{

	class AnimationInfo
		: public std::enable_shared_from_this<AnimationInfo>
	{
	public:
		std::string m_animationName;
		std::wstring m_animationPath;
		bool m_isChangesOnFinish;
		std::vector<std::shared_ptr<AnimationInfo>> m_nextStates;

		int m_id;
		static uint32 m_stateCount;

	public:
		AnimationInfo();
		~AnimationInfo();

	public:
		bool HasTransition() { return !m_nextStates.empty(); }
		void AddTransition(std::shared_ptr<AnimationInfo> _nextState);
		void DeleteTransition(std::shared_ptr<AnimationInfo> _entity);
	};

	struct AnimeInfo
	{
		std::string name = "Empty";
		GraphEditor::TemplateIndex templateIndex = 0;
		float x = 0;
		float y = 0;
		bool isSelected = false;

		std::wstring path;
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

	public:
		PROPERTY(states);
		//std::vector<std::string> m_states;
		std::vector<std::shared_ptr<AnimationInfo>> m_states;
		std::shared_ptr <AnimationInfo> m_currentState;

		std::vector<AnimeInfo> m_infos;

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

	public:
		void AddState(std::shared_ptr<AnimationInfo> _state);


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