#pragma once
#include "ButtonBehavior.h"
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

namespace Truth
{
	struct Texture;
}

namespace Ideal
{
	class ISprite;
}

class Player;

class PlayerHPUI :
	public Truth::ButtonBehavior
{
	GENERATE_CLASS_TYPE_INFO(PlayerHPUI);

private:
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER();
	template<class Archive>
	void save(Archive& ar, const unsigned int file_version) const;
	template<class Archive>
	void load(Archive& ar, const unsigned int file_version);

private:
	std::weak_ptr<Player> m_player;

	std::shared_ptr<Truth::Texture> m_HPTexture[6];

	std::shared_ptr<Ideal::ISprite> m_underSprite;
	std::shared_ptr<Ideal::ISprite> m_upSprite;

	const Vector2 m_size = { 550, 78 };
	const Vector2 m_position = { 391, 981 };

public:
	PlayerHPUI();
	virtual ~PlayerHPUI();
	virtual void Update() override;
	virtual void Start() override;

private:
	void SetSampling(const Vector2& _min, const Vector2& _max);
	void SetScale(const Vector2& _scale);
};

template<class Archive>
void PlayerHPUI::save(Archive& _ar, const unsigned int file_version) const
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}

template<class Archive>
void PlayerHPUI::load(Archive& _ar, const unsigned int file_version)
{
	_ar& boost::serialization::base_object<ButtonBehavior>(*this);
}
BOOST_CLASS_EXPORT_KEY(PlayerHPUI)
BOOST_CLASS_VERSION(PlayerHPUI, 0)