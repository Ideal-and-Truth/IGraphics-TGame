#include "TimeManager.h"

TimeManager::TimeManager()
	:m_deltaTime(0.0f)
	, m_currentCount{}
	, m_prevCount{}
	, m_frequency{}
{
	// 현재 카운트
	QueryPerformanceCounter(&m_prevCount);

	// 초당 카운트 횟수 (천만)
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

	// 이전 프레임의 카운팅과 현재 프레임 카운팅 값의 차이를 구한다.
	m_deltaTime = static_cast<float4>(m_currentCount.QuadPart - m_prevCount.QuadPart) / 
		static_cast<float4>(m_frequency.QuadPart);

	// 디버깅시에는 최대 프레임 제한
#ifdef _DEBUG
	if (m_deltaTime > 0.16f)
	{
		m_deltaTime = 0.16f;
	}
#endif // _DEBUG

	// 이전카운트 값을 현재값으로 갱신 (다음번에 계산을 위해서)
	m_prevCount = m_currentCount;
}
