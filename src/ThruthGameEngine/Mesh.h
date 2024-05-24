#pragma once
#include "Component.h"
namespace Truth
{
	class Mesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Mesh)

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
		void Update(std::any _p);

	private:
		METHOD(Awake);
		void Awake();
	};
}

