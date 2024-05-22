#include "Entity.h"
#include "Transform.h"

uint32 Truth::Entity::m_entityCount = 0;

Truth::Entity::Entity()
	: m_manager()
	, m_name("Empty Enitity")
	, m_ID(m_entityCount++)
	, m_layer(0)
	, m_tag("None")
{
}

Truth::Entity::~Entity()
{
	m_components.clear();
}

void Truth::Entity::Initailize()
{
	m_transform = AddComponent<Transform>();
}

void Truth::Entity::SetPosition(Vector3 _pos) const
{
	m_transform->SetPosition(_pos);
}

DirectX::SimpleMath::Vector3 Truth::Entity::GetPosition() const
{
	return m_transform->m_position;
}

void Truth::Entity::Awake()
{
	Initailize();
	for (auto& c : m_components)
	{
		auto met = c->GetTypeInfo().GetMethod("Awake");
		if (met)
		{
			met->Invoke<void>(c.get());
		}
	}
	// DEBUG_PRINT(Truth::Entity::StaticTypeInfo().Dump(this).c_str());
}

void Truth::Entity::Destroy()
{
	for (auto& p : m_destroy)
	{
		p.second->Invoke<void>(p.first);
	}
}

void Truth::Entity::Start()
{
	for (auto& c : m_components)
	{
		auto met = c->GetTypeInfo().GetMethod("Start");
		if (met)
		{
			met->Invoke<void>(c.get());
		}
	}
}

void Truth::Entity::Update()
{
	for (auto& p : m_update)
	{
		p.second->Invoke<void>(p.first);
	}
}

void Truth::Entity::OnCollisionEnter(Collider* _other)
{
	for (auto& p : m_onCollisionEnter)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

void Truth::Entity::OnCollisionStay(Collider* _other)
{
	for (auto& p : m_onCollisionStay)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

void Truth::Entity::OnCollisionExit(Collider* _other)
{
	for (auto& p : m_onCollisionExit)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

void Truth::Entity::OnTriggerEnter(Collider* _other)
{
	for (auto& p : m_onTriggerEnter)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

void Truth::Entity::OnTriggerStay(Collider* _other)
{
	for (auto& p : m_onTriggerStay)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

void Truth::Entity::OnTriggerExit(Collider* _other)
{
	for (auto& p : m_onTriggerExit)
	{
		p.second->Invoke<void>(p.first, _other);
	}
}

