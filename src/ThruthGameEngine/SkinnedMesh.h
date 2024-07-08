#pragma once
#include "Component.h"
namespace Truth
{
	class SkinnedMesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(SkinnedMesh);

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Ideal::ISkinnedMeshObject> m_skinnedMesh;
		std::shared_ptr<Ideal::IAnimation> m_animation;

		PROPERTY(path);
		std::wstring m_path;

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(currentFrame);
		int m_currentFrame;

		PROPERTY(isAnimationStart);
		bool m_isAnimationStart;

		PROPERTY(isAnimationEnd);
		bool m_isAnimationEnd;

		PROPERTY(isAnimationChanged);
		bool m_isAnimationChanged;

	public:
		SkinnedMesh();
		SkinnedMesh(std::wstring _path);
		virtual ~SkinnedMesh();

		void SetSkinnedMesh(std::wstring _path);
		void AddAnimation(std::string _name, std::wstring _path);
		void SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished);
		void SetRenderable(bool _isRenderable);

		METHOD(Initalize);
		void Initalize();

		METHOD(FixedUpdate);
		void FixedUpdate();

		METHOD(Update);
		void Update();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef _DEBUG
		virtual void EditorSetValue();
#endif // _DEBUG

	};

	template<class Archive>
	void Truth::SkinnedMesh::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
	}

	template<class Archive>
	void Truth::SkinnedMesh::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::SkinnedMesh)
BOOST_CLASS_VERSION(Truth::SkinnedMesh, 0)