#include "GraphicsManager.h"

Truth::GraphicsManager::GraphicsManager()
	: m_renderer(nullptr)
	, m_renderScene(nullptr)
{

}

Truth::GraphicsManager::~GraphicsManager()
{

}

void Truth::GraphicsManager::Initalize(HWND _hwnd, uint32 _wight, uint32 _height)
{
	m_renderer = CreateRenderer(
		EGraphicsInterfaceType::D3D12,
		&_hwnd,
		_wight,
		_height,
		m_assetPath[0],
		m_assetPath[1],
		m_assetPath[2]
	);

	m_renderer->Init();
}

void Truth::GraphicsManager::Finalize()
{

}

void Truth::GraphicsManager::Render()
{
	m_renderer->Render();
}

void Truth::GraphicsManager::AddObject(std::shared_ptr<EMesh>)
{

}
