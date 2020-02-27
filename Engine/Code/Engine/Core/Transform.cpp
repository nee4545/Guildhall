
#include "Engine/Core/Transform.hpp"

Transform::Transform()
{
	m_scale = Vec3( 1.f , 1.f , 1.f );
}

void Transform::SetPosition( Vec3 pos )
{
	m_position = pos;
}

void Transform::SetRotationFromPitchRollYawDegrees( float pitch , float roll , float yaw )
{
	m_rotationPitchRollYawDegrees.x = yaw;
	m_rotationPitchRollYawDegrees.y = pitch;
	m_rotationPitchRollYawDegrees.z = roll;
}

void Transform::Translate( Vec3 translation )
{
	m_position += translation;
}

Mat44 Transform::ToMatrix() const
{

	Mat44 toReturn = Mat44::CreateFromScaleRotationPosition( m_scale , m_rotationPitchRollYawDegrees , m_position );
	return toReturn;
}
