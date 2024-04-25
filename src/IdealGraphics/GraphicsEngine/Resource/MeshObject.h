#pragma once
#include "GraphicsEngine/public/IMeshObject.h"

#include "Core/Core.h"

namespace Ideal
{
	class IdealRenderer;
	class Model;
}

namespace Ideal
{
	class MeshObject : public Ideal::IMeshObject
	{
	public:
		MeshObject();
		virtual ~MeshObject();

	public:
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Matrix) override;

	public:
		void Tick(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		void Render(std::shared_ptr<Ideal::IdealRenderer> Renderer);
		void SetModel(std::shared_ptr<Ideal::Model> Model);

	private:
		std::shared_ptr<Ideal::Model> m_model;

		DirectX::SimpleMath::Matrix m_transformMatrix;
	};
}