#include "Transform.h"
#include "Managers.h"


Truth::Transform::Transform()
	: Component()
	, m_position(0.0f, 0.0f, 0.0f)
	, m_scale(1.0f, 1.0f, 1.0f)
	, m_rotation{}
	, m_transformMatrix{Matrix::Identity}
{
	m_canMultiple = false;
	m_name = typeid(*this).name();
}

Truth::Transform::~Transform()
{
}

/// <summary>
/// ��ȯ ������ ���ͷ� ���� �� �� �ѹ��� �����Ѵ�.
/// �׶��׶� �����ϴ°� ���� �̰� �� ������ �ƴ��� �����
/// </summary>
void Truth::Transform::ApplyTransform(std::any _p)
{
	Matrix scaleMT = Matrix::CreateScale(m_scale);
	Matrix rotationMT = Matrix::CreateFromQuaternion(m_rotation);
	Matrix traslationMT = Matrix::CreateTranslation(m_position);

	m_transformMatrix = scaleMT * rotationMT * traslationMT;
}

void Truth::Transform::Awake()
{
	EventBind("Apply Transform", &Transform::ApplyTransform);
}

