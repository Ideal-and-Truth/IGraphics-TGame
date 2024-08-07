#include "Entity.h"
#include "Transform.h"

uint32 Truth::Entity::m_entityCount = 0;

Truth::Entity::Entity(std::shared_ptr<Managers> _mangers)
	: m_manager(_mangers)
	, m_name("Empty Enitity")
	, m_ID(m_entityCount++)
	, m_layer(0)
	, m_tag("None")
	, m_index(-1)
	, m_parent(std::weak_ptr<Entity>())
{
	m_transform = std::make_shared<Transform>();
	m_components.push_back(m_transform);
}

Truth::Entity::Entity()
	: m_layer()
	, m_index()
	, m_ID(m_entityCount++)
	, m_parent(std::weak_ptr<Entity>())
{
}

Truth::Entity::~Entity()
{
	m_components.clear();
	m_children.clear();
}

void Truth::Entity::Initailize()
{
	m_transform = GetComponent<Transform>().lock();
	int32 index = 0;
	for (auto& c : m_components)
	{
		c->SetOwner(shared_from_this());
		c->SetManager(m_manager);

		// c->Initalize();
		ApplyComponent(c);
		c->m_index = index++;
	}
}

void Truth::Entity::SetPosition(const Vector3& _pos) const
{
	m_transform->SetPosition(_pos);
}

void Truth::Entity::SetScale(const Vector3& _scale) const
{
	m_transform->SetScale(_scale);
}

const DirectX::SimpleMath::Vector3& Truth::Entity::GetLocalPosition() const
{
	return m_transform->m_position;
}

const DirectX::SimpleMath::Quaternion& Truth::Entity::GetLocalRotation() const
{
	return m_transform->m_rotation;
}

const DirectX::SimpleMath::Vector3& Truth::Entity::GetLocalScale() const
{
	return m_transform->m_scale;
}

const DirectX::SimpleMath::Vector3& Truth::Entity::GetWorldPosition() const
{
	return m_transform->m_worldPosition;
}

const DirectX::SimpleMath::Quaternion& Truth::Entity::GetWorldRotation() const
{
	return m_transform->m_worldRotation;
}

const DirectX::SimpleMath::Vector3& Truth::Entity::GetWorldScale() const
{
	return m_transform->m_worldScale;
}

void Truth::Entity::ApplyTransform()
{
	m_transform->ApplyTransform();
	for (auto p = m_applyTransform.begin(); p != m_applyTransform.end() ; p++)
	{
		if (p->first.expired())
		{
			if (p == m_applyTransform.end() - 1)
			{
				m_applyTransform.pop_back();
				return;
			}
			auto temp = m_applyTransform.end() - 1;
			std::iter_swap(p, temp);
			m_applyTransform.pop_back();

			if (m_applyTransform.empty())
			{
				return;
			}
		}

		if (p->first.lock()->m_name == "Transform")
		{
			continue;
		}
		p->second->Invoke<void>(p->first.lock().get());
	}
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
	IterateComponentMethod(m_destroy);
	m_components.clear();
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
	IterateComponentMethod(m_update);
}

void Truth::Entity::FixedUpdate()
{
	IterateComponentMethod(m_fixedUpdate);
}

void Truth::Entity::LateUpdate()
{
	IterateComponentMethod(m_latedUpdate);
}

void Truth::Entity::OnCollisionEnter(Collider* _other)
{
	IterateComponentMethod(m_onCollisionEnter, _other);
}

void Truth::Entity::OnCollisionStay(Collider* _other)
{
	IterateComponentMethod(m_onCollisionStay, _other);
}

void Truth::Entity::OnCollisionExit(Collider* _other)
{
	IterateComponentMethod(m_onCollisionExit, _other);
}

void Truth::Entity::OnTriggerEnter(Collider* _other)
{
	IterateComponentMethod(m_onTriggerEnter, _other);
}

void Truth::Entity::OnTriggerStay(Collider* _other)
{
	IterateComponentMethod(m_onTriggerStay, _other);
}

void Truth::Entity::OnTriggerExit(Collider* _other)
{
	IterateComponentMethod(m_onTriggerExit, _other);
}

void Truth::Entity::DeleteComponent(int32 _index)
{
	if (_index >= m_components.size())
	{
		return;
	}

	m_components.back()->m_index = _index;
	std::iter_swap(m_components.begin() + _index, m_components.begin() + (m_components.size() - 1));
	m_components.pop_back();
}

void Truth::Entity::AddComponent(std::shared_ptr<Component> _component)
{
	_component->SetManager(m_manager);
	_component->SetOwner(shared_from_this());
	_component->m_index = static_cast<int32>(m_components.size());
	m_components.push_back(_component);
	_component->Initalize();
	ApplyComponent(_component);
}

void Truth::Entity::AddChild(std::shared_ptr<Entity> _entity)
{
	m_children.push_back(_entity);
	_entity->m_parent = shared_from_this();
}

void Truth::Entity::DeleteChild(std::shared_ptr<Entity> _entity)
{
	for (auto c = m_children.begin(); c != m_children.end() ; c++)
	{
		if ((*c) == _entity)
		{
			m_children.erase(c);
			return;
		}
	}
}

const DirectX::SimpleMath::Matrix& Truth::Entity::GetWorldTM() const
{
	return m_transform->m_globalTM;
}

void Truth::Entity::SetWorldTM(const Matrix& _tm) const
{
	m_transform->m_globalTM = _tm;
}

void Truth::Entity::ApplyComponent(std::shared_ptr<Component> _c)
{
	_c->SetOwner(shared_from_this());
	_c->SetManager(m_manager);
	const auto& mets = _c->GetTypeInfo().GetMethods();

	for (const auto& m : mets)
	{
		std::string metName = m->GetName();

		auto p = std::make_pair(_c, m);

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

		if (metName == "ApplyTransform")
		{
			m_applyTransform.push_back(p);
		}

		if (metName == "Initalize")
		{
			m->Invoke<void>(_c.get());
		}
	}
}

void Truth::Entity::IterateComponentMethod(ComponentMethod& _cm)
{
	for (auto p = _cm.begin(); p != _cm.end(); p++)
	{
		if (p->first.expired())
		{
			auto temp = _cm.end() - 1;
			std::iter_swap(p, temp);
			_cm.pop_back();
			p = temp;

			if (_cm.empty())
			{
				return;
			}
		}
		p->second->Invoke<void>(p->first.lock().get());
	}
}

void Truth::Entity::IterateComponentMethod(ComponentMethod& _cm, Collider* _param)
{
	for (auto p = _cm.begin(); p != _cm.end(); p++)
	{
		if (p->first.expired())
		{
			auto temp = _cm.end() - 1;
			std::iter_swap(p, temp);
			_cm.pop_back();
			p = temp;

			if (_cm.empty())
			{
				return;
			}
		}
		p->second->Invoke<void>(p->first.lock().get(), _param);
	}
}
