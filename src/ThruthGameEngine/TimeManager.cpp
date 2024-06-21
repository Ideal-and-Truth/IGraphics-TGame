#include "TimeManager.h"
#include "Managers.h"
#include "EventManager.h"

Truth::TimeManager::TimeManager()
	: m_deltaTime(0.0f)
	, m_time(0.0f)
	, m_timeScale(1.0f)
	, m_fixedDeltaTime(0.0f)
	, m_currentCount{}
	, m_prevCount{}
	, m_frequency{}
	, m_eventManager()
	, m_managers()
	, m_absDeltaTime(0.0f)
{
	DEBUG_PRINT("Create TimeManager\n");

	// ���� ī��Ʈ
	QueryPerformanceCounter(&m_prevCount);

	// �ʴ� ī��Ʈ Ƚ�� (õ��)
	QueryPerformanceFrequency(&m_frequency);
}

Truth::TimeManager::~TimeManager()
{
	DEBUG_PRINT("Finalize TimeManager\n");
}

void Truth::TimeManager::Initalize(std::shared_ptr<Managers> _managers)
{
	m_managers = _managers;
	m_eventManager = m_managers.lock()->Event();
}

/// <summary>
/// �ð� ������Ʈ
/// </summary>
void Truth::TimeManager::Update()
{
	// ���� �������� ī���ð� ���� ������ ī���� ���� ���̸� ���Ѵ�.
	QueryPerformanceCounter(&m_currentCount);
	float delta = static_cast<float4>(m_currentCount.QuadPart - m_prevCount.QuadPart) /
		static_cast<float4>(m_frequency.QuadPart);

	// �����ÿ��� �ּ� ������ ����
#ifdef _DEBUG
	if (delta > 0.16f)
	{
		delta = 0.16f;
	}
#endif // _DEBUG

	m_absDeltaTime = delta;

	// ���� ������ �ð�
	m_deltaTime = delta * m_timeScale;
	// ���� ������ �ð�
	m_fixedDeltaTime += delta * m_timeScale;

	if (m_managers.lock()->m_isEdit)
	{
		m_fixedDeltaTime = 0.0f;
	}

	// ���� ���� ������ �ð��� ������ �ѱ�� �Ǹ� �̺�Ʈ�� �����Ѵ�.
	while (m_fixedDeltaTime >= m_fixedTime)
	{
		m_managers.lock()->FixedUpdate();
		// �ð� ����
		m_fixedDeltaTime -= m_fixedTime;
	}

	// ��ü �ð�
	m_time += delta * m_timeScale;

	// ����ī��Ʈ ���� ���簪���� ���� (�������� ����� ���ؼ�)
	m_prevCount = m_currentCount;
}

void Truth::TimeManager::Finalize()
{

}
