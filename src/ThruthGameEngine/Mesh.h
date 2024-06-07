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

		PROPERTY(path)
		std::wstring m_path;

	public:
		Mesh();
		Mesh(std::wstring _path);
		virtual ~Mesh();

		void SetMesh(std::wstring _path);
		void SetRenderable(bool _isRenderable);

		METHOD(Initalize);
		void Initalize();

		METHOD(Update);
		void Update();

	private:
		METHOD(Awake);
		void Awake();
	};

	template<class Archive>
	void Truth::Mesh::serialize(Archive& _ar, const unsigned int _file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
	}

}

