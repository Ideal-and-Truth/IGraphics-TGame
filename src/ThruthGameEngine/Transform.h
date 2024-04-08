#pragma once
#include "Headers.h"
#include "Component.h"

class Transform :
    public Component
{
public:
	Vector3 m_position;
	Vector3 m_scale;
	Quaternion m_rotation;

public:
    Transform();
    virtual ~Transform();

    void Update(float4 _dt) override;
    void Render() override;
    void LateUpdate(float4 _dt) override;
    void FiexUpdate(float4 _dt) override;
};

