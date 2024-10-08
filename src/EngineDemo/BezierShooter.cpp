#include "BezierShooter.h"
#include "Managers.h"
#include "GraphicsManager.h"
#include "IMeshObject.h"
#include "IMesh.h"
#include "MathUtil.h"

BOOST_CLASS_EXPORT_IMPLEMENT(BezierShooter)

BezierShooter::BezierShooter()
	: m_endPoint{ 0.0f, 0.0f, 0.0f }
	, m_controlPoint{ 0.0f, 0.0f, 0.0f }
	, m_slope(1.0f)
	, m_timer(0.0f)
{
	m_name = "BezierShooter";
	// m_drawPoints.resize(3);
}

BezierShooter::~BezierShooter()
{
	for (auto& m : m_drawPoints)
	{
		m_managers.lock()->Graphics()->DeleteMeshObject(m);
	}
	m_managers.lock()->Graphics()->DeleteMeshObject(m_obj);
}

void BezierShooter::Initalize()
{
	for (uint32 i = 0; i < 3; i++)
	{
		m_drawPoints.push_back(m_managers.lock()->Graphics()->CreateMesh(L"DebugObject/debugCube"));
		auto mat = m_managers.lock()->Graphics()->CraeteMaterial("DebugMat");

		m_drawPoints[i]->GetMeshByIndex(0).lock()->SetMaterialObject(mat->m_material);
		m_drawPoints[i]->SetDrawObject(true);
	}

	m_obj = m_managers.lock()->Graphics()->CreateMesh(L"DebugObject/debugCube");
	auto mat = m_managers.lock()->Graphics()->CraeteMaterial("DebugMat");
	m_obj->GetMeshByIndex(0).lock()->SetMaterialObject(mat->m_material);
	m_obj->SetDrawObject(true);
}


void BezierShooter::Awake()
{
}
void BezierShooter::Update()
{
	m_points.clear();
	m_points.push_back(m_owner.lock()->GetWorldPosition());
	m_points.push_back(m_controlPoint);
	m_points.push_back(m_endPoint);

	m_timer += GetDeltaTime();
	while (m_timer >= 1.0f)
	{
		m_timer -= 1.0f;
	}

	m_objPoint = MathUtil::GetCatmullPoint(m_points, m_timer);
	m_obj->SetTransformMatrix(Matrix::CreateTranslation(m_objPoint));
}

void BezierShooter::ApplyTransform()
{
	m_drawPoints[0]->SetTransformMatrix(Matrix::CreateTranslation(m_owner.lock()->GetWorldPosition()));
	m_drawPoints[1]->SetTransformMatrix(Matrix::CreateTranslation(m_controlPoint));
	m_drawPoints[2]->SetTransformMatrix(Matrix::CreateTranslation(m_endPoint));
}
