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

void Truth::TimeManager::Initalize(std::shared_ptr<EventManager> _eventManager)
{
	m_eventManager = _eventManager;
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

	// ���� ������ �ð�
	m_deltaTime = delta * m_timeScale;
	// ���� ������ �ð�
	m_fixedDeltaTime += delta * m_timeScale;

	// ���� ���� ������ �ð��� ������ �ѱ�� �Ǹ� �̺�Ʈ�� �����Ѵ�.
	while (m_fixedDeltaTime >= m_fixedTime)
	{
		m_eventManager.lock()->PublishEvent("FixedUpdate");
		m_eventManager.lock()->Update();
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
