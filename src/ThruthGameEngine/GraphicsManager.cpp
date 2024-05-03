#include "GraphicsManager.h"
#include "IRenderScene.h"
#include "ISkinnedMeshObject.h"

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
	m_renderScene = m_renderer->CreateRenderScene();
	m_renderer->SetRenderScene(m_renderScene);
}

void Truth::GraphicsManager::Finalize()
{

}

void Truth::GraphicsManager::Render()
{
	m_renderer->Render();
}

void Truth::GraphicsManager::AddObject(std::shared_ptr<Ideal::ISkinnedMeshObject> _mesh)
{
	m_renderScene->AddObject(_mesh);
}

std::shared_ptr<Ideal::ISkinnedMeshObject> Truth::GraphicsManager::CreateSkinnedMesh(std::wstring _path)
{
	return m_renderer->CreateSkinnedMeshObject(_path);
}

