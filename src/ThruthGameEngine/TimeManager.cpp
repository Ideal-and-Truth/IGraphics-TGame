#include "TimeManager.h"

TimeManager::TimeManager()
	:m_deltaTime(0.0f)
	, m_currentCount{}
	, m_prevCount{}
	, m_frequency{}
{
	// ���� ī��Ʈ
	QueryPerformanceCounter(&m_prevCount);

	// �ʴ� ī��Ʈ Ƚ�� (õ��)
	QueryPerformanceFrequency(&m_frequency);
}

TimeManager::~TimeManager()
{
}

void TimeManager::Initalize()
{

}

void TimeManager::Update()
{
	QueryPerformanceCounter(&m_currentCount);

	// ���� �������� ī���ð� ���� ������ ī���� ���� ���̸� ���Ѵ�.
	m_deltaTime = static_cast<float4>(m_currentCount.QuadPart - m_prevCount.QuadPart) / 
		static_cast<float4>(m_frequency.QuadPart);

	// �����ÿ��� �ִ� ������ ����
#ifdef _DEBUG
	if (m_deltaTime > 0.16f)
	{
		m_deltaTime = 0.16f;
	}
#endif // _DEBUG

	// ����ī��Ʈ ���� ���簪���� ���� (�������� ����� ���ؼ�)
	m_prevCount = m_currentCount;
}
