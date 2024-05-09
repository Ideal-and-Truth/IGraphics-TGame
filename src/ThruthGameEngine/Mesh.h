#pragma once
#include "Component.h"
namespace Truth
{
	class Mesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Mesh)

	private:
		std::shared_ptr<Ideal::ISkinnedMeshObject> m_mesh;

		PROPERTY(path)
		std::wstring m_path;

	public:
		Mesh(std::shared_ptr<Managers> _managers);
		Mesh(std::shared_ptr<Managers> _managers, std::wstring _path);
		virtual ~Mesh();

		void SetMesh(std::wstring _path);
		void SetRenderable(bool _isRenderable);
		void Update(std::any _p);

	private:

	};
}

