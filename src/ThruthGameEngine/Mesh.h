#pragma once
#include "Component.h"

namespace Ideal
{
	class IMeshObject;
	class IMesh;
}

/// <summary>
/// Static Mehs Class
/// </summary>
namespace Truth
{
	class Mesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Mesh);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::shared_ptr<Ideal::IMeshObject> m_mesh;
		std::vector<std::shared_ptr<Ideal::IMesh>> m_subMesh;

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

		void DeleteMesh();

		void SetMaterialByIndex(uint32 _index, std::string _material);
		std::vector<uint64> GetMaterialImagesIDByIndex(uint32 _index);

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};

	template<class Archive>
	void Truth::Mesh::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
	}

	template<class Archive>
	void Truth::Mesh::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::Mesh)
BOOST_CLASS_VERSION(Truth::Mesh, 0)