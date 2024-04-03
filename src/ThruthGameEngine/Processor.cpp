#include "Processor.h"
#include "TimeManager.h"
#include "InputManager.h"

Processor::Processor()
{

}

Processor::~Processor()
{

}

void Processor::Initialize()
{
	m_timeManager = std::make_unique<TimeManager>();
	m_inputManager = std::make_unique<InputManager>();
}

void Processor::Process()
{
	Update();
	FixedUpdate();
	LateUpdate();
	Render();
}

void Processor::Update()
{

}

void Processor::LateUpdate()
{

}

void Processor::FixedUpdate()
{

}

void Processor::Render()
{

}
