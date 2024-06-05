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

		PROPERTY(isRendering)
		bool m_isRendering;

	public:
		Mesh();
		Mesh(std::wstring _path);
		virtual ~Mesh();

		void SetMesh(std::wstring _path);
		void SetRenderable(bool _isRenderable);

		METHOD(Update);
		void Update();

	private:
		METHOD(Awake);
		void Awake();
	};
}

