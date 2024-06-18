#pragma once
#include "Component.h"
namespace Truth
{
	class Mesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Mesh);
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive& _ar, const unsigned int _file_version);

	private:
		std::shared_ptr<Ideal::IMeshObject> m_mesh;

		PROPERTY(path);
		std::wstring m_path;

		PROPERTY(isRendering);
		bool m_isRendering;

	public:
		Mesh();
		Mesh(std::wstring _path);
		virtual ~Mesh();

		void SetMesh(std::wstring _path);
		void SetRenderable(bool _isRenderable);

		METHOD(Initalize);
		void Initalize();

		METHOD(ApplyTransform);
		void ApplyTransform();

#ifdef _DEBUG
		virtual void EditorSetValue();
#endif // _DEBUG

	};

	template<class Archive>
	void Truth::Mesh::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
	}

}

BOOST_CLASS_EXPORT_KEY(Truth::Mesh)