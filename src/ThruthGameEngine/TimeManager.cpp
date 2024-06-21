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

	// 현재 카운트
	QueryPerformanceCounter(&m_prevCount);

	// 초당 카운트 횟수 (천만)
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
/// 시간 업데이트
/// </summary>
void Truth::TimeManager::Update()
{
	// 이전 프레임의 카운팅과 현재 프레임 카운팅 값의 차이를 구한다.
	QueryPerformanceCounter(&m_currentCount);
	float delta = static_cast<float4>(m_currentCount.QuadPart - m_prevCount.QuadPart) /
		static_cast<float4>(m_frequency.QuadPart);

	// 디버깅시에는 최소 프레임 제한
#ifdef _DEBUG
	if (delta > 0.16f)
	{
		delta = 0.16f;
	}
#endif // _DEBUG

	m_absDeltaTime = delta;

	// 현재 프레임 시간
	m_deltaTime = delta * m_timeScale;
	// 고정 프레임 시간
	m_fixedDeltaTime += delta * m_timeScale;

	if (m_managers.lock()->m_isEdit)
	{
		m_fixedDeltaTime = 0.0f;
	}

	// 만일 고정 프레임 시간이 단위를 넘기게 되면 이벤트를 발행한다.
	while (m_fixedDeltaTime >= m_fixedTime)
	{
		m_managers.lock()->FixedUpdate();
		// 시간 조절
		m_fixedDeltaTime -= m_fixedTime;
	}

	// 전체 시간
	m_time += delta * m_timeScale;

	// 이전카운트 값을 현재값으로 갱신 (다음번에 계산을 위해서)
	m_prevCount = m_currentCount;
}

void Truth::TimeManager::Finalize()
{

}
