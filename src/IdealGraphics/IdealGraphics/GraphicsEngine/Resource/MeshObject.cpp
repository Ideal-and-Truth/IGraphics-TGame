#include "MeshObject.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/Resource/Model.h"
#include "GraphicsEngine/Resource/Bone.h"
#include "GraphicsEngine/Resource/Mesh.h"
#include "GraphicsEngine/Resource/Material.h"
#include "GraphicsEngine/D3D12Renderer.h"

using namespace Ideal;

MeshObject::MeshObject()
{

}

MeshObject::~MeshObject()
{

}

void MeshObject::SetTransformMatrix(const Matrix& Matrix)
{
	m_model->SetTransformMatrix(Matrix);
}

void MeshObject::Tick(std::shared_ptr<IdealRenderer> Renderer)
{
	if (m_model == nullptr)
	{
		return;
	}

	m_model->Tick(std::static_pointer_cast<D3D12Renderer>(Renderer));
}

void MeshObject::Render(std::shared_ptr<IdealRenderer> Renderer)
{
	m_model->Render(std::static_pointer_cast<D3D12Renderer>(Renderer));
}

void MeshObject::SetModel(std::shared_ptr<Ideal::Model> Model)
{
	m_model = Model;
}
