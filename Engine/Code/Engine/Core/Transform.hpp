#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"

class Transform
{
public:
	Transform();
	~Transform() {};
	void SetPosition( Vec3 pos );
	void SetRotationFromPitchRollYawDegrees( float pitch , float roll , float yaw );
	void Translate( Vec3 translation );

	Mat44 ToMatrix() const;

public:
	Vec3 m_position;
	Vec3 m_rotationPitchRollYawDegrees;
	Vec3 m_scale;
};