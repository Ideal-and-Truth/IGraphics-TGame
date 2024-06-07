#include "Entity.h"
#include "Transform.h"

uint32 Truth::Entity::m_entityCount = 0;

Truth::Entity::Entity(std::shared_ptr<Managers> _mangers)
	: m_manager(_mangers)
	, m_name("Empty Enitity")
	, m_ID(m_entityCount++)
	, m_layer(0)
	, m_tag("None")
{
	m_transform = std::make_shared<Transform>();
	m_components.push_back(m_transform);
}

Truth::Entity::~Entity()
{
	m_components.clear();
}

void Truth::Entity::Initailize()
{
	m_transform = GetComponent<Transform>().lock();
	for (auto& c : m_components)
	{
		c->SetOwner(shared_from_this());
		c->SetManager(m_manager);

		const auto& mets = c->GetTypeInfo().GetMethods();

		for (const auto& m : mets)
		{
			std::string metName = m->GetName();

			auto p = std::make_pair(c.get(), m);

			if (metName == "OnCollisionEnter")
			{
				m_onCollisionEnter.push_back(p);
			}
			if (metName == "OnCollisionStay")
			{
				m_onCollisionStay.push_back(p);
			}
			if (metName == "OnCollisionExit")
			{
				m_onCollisionExit.push_back(p);
			}

			if (metName == "OnTriggerEnter")
			{
				m_onTriggerEnter.push_back(p);
			}
			if (metName == "OnTriggerStay")
			{
				m_onTriggerStay.push_back(p);
			}
			if (metName == "OnTriggerExit")
			{
				m_onTriggerExit.push_back(p);
			}

			if (metName == "Update")
			{
				m_update.push_back(p);
			}
			if (metName == "FixedUpdate")
			{
				m_fixedUpdate.push_back(p);
			}
			if (metName == "LateUpdate")
			{
				m_latedUpdate.push_back(p);
			}

			if (metName == "Destroy")
			{
				m_destroy.push_back(p);
			}

			if (metName == "Initalize")
			{
				m->Invoke<void>(c.get());
			}
		}
	}
}

void Truth::Entity::SetPosition(Vector3 _pos) const
{
	m_transform->SetPosition(_pos);
}

void Truth::Entity::SetScale(Vector3 _scale) const
{
	m_transform->SetScale(_scale);
}

DirectX::SimpleMath::Vector3 Truth::Entity::GetPosition() const
{
	return m_transform->m_position;
}

DirectX::SimpleMath::Quaternion Truth::Entity::GetRotation() const
{
	return m_transform->m_rotation;
}

void Truth::Entity::ApplyTransform() const
{
	m_transform->ApplyTransform();
}

void Truth::Entity::Awake()
{
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

DirectX::SimpleMath::Matrix Truth::Entity::GetWorldTM() const
{
	return m_transform->m_transformMatrix;
}

