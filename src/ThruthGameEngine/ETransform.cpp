#include "ETransform.h"
#include "Managers.h"

REFLECT_STRUCT_BEGIN(ETransform)
REFLECT_STRUCT_MEMBER(m_name)
REFLECT_STRUCT_END()

ETransform::ETransform()
	: m_position(0.0f, 0.0f, 0.0f)
	, m_scale(1.0f, 1.0f, 1.0f)
	, m_rotation{}
	, m_transformMatrix{Matrix::Identity}
{
	m_canMultiple = false;
	m_name = typeid(*this).name();
}

ETransform::~ETransform()
{

}

void ETransform::Awake()
{
	EventBind("Apply Transform", &ETransform::ApplyTransform);
}

/// <summary>
/// ��ȯ ������ ���ͷ� ���� �� �� �ѹ��� �����Ѵ�.
/// �׶��׶� �����ϴ°� ���� �̰� �� ������ �ƴ��� �����
/// </summary>
void ETransform::ApplyTransform(std::any _p)
{
	Matrix scaleMT = Matrix::CreateScale(m_scale);
	Matrix rotationMT = Matrix::CreateFromQuaternion(m_rotation);
	Matrix traslationMT = Matrix::CreateTranslation(m_position);

	m_transformMatrix = scaleMT * rotationMT * traslationMT;
}


