#pragma once
#include "Component.h"
#include "SkinnedMesh.h"


// �ִϸ����� ��� �ӽ÷� ���� ������Ʈ
class PlayerAnimator :
	public Truth::Component
{
	GENERATE_CLASS_TYPE_INFO(PlayerAnimator);
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& _ar, const unsigned int _file_version);

private:
	bool m_isWalking;
	bool m_isRunning;
	std::shared_ptr<Truth::SkinnedMesh> m_skinnedMesh;

public:
	PlayerAnimator();
	virtual ~PlayerAnimator();

	METHOD(Awake);
	void Awake();

	METHOD(Start);
	void Start();

	METHOD(Update);
	void Update();

private:
	void PlayerIdle(const void*);
	void PlayerWalk(const void*);
	void PlayerAttack(const void*);

};

template<class Archive>
void PlayerAnimator::serialize(Archive& _ar, const unsigned int _file_version)
{
	_ar& boost::serialization::base_object<Component>(*this);
}

BOOST_CLASS_EXPORT_KEY(PlayerAnimator)