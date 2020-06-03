
#include "Engine/Core/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"

Transform::Transform()
{
	m_scale = Vec3( 1.f , 1.f , 1.f );
}

void Transform::SetPosition( Vec3 pos )
{
	m_position = pos;
}

void Transform::SetRotationFromYawPitchRollDegrees( float yaw , float pitch , float roll )
{
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;

	/*m_rotationPitchRollYawDegrees.x = Clamp( m_rotationPitchRollYawDegrees.x , 0.f , 360.f );
	m_rotationPitchRollYawDegrees.y = Clamp( m_rotationPitchRollYawDegrees.y , 0.f , 360.f );
	m_rotationPitchRollYawDegrees.z = Clamp( m_rotationPitchRollYawDegrees.z , 0.f , 360.f );*/
}

void Transform::Translate( Vec3 translation )
{
	m_position += translation;
}

void Transform::SetConvenstion( Convensions convension )
{
	m_convention = convension;
}

Mat44 Transform::ToMatrix() const
{
	
	Mat44 toReturn = Mat44::CreateFromScaleRotationPosition( m_scale , m_pitch, m_yaw, m_roll , m_position, m_convention );
	return toReturn;

}
