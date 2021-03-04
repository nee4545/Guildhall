#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/EngineCommon.hpp"


class Transform
{
public:
	Transform();
	~Transform() {};
	void SetPosition( Vec3 pos );
	void SetRotationFromYawPitchRollDegrees( float yaw , float pitch, float roll );
	void Translate( Vec3 translation );
	void SetConvenstion( Convensions convension );

	Mat44 ToMatrix() const;

public:
	Vec3 m_position;
	Vec3 m_scale;
	float m_pitch=0.f;
	float m_yaw=0.f;
	float m_roll=0.f;
	Convensions m_convention = CONVENTION_XRIGHT_YUP_ZBACK;
};