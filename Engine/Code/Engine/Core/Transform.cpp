
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

void Transform::SetRotationFromPitchRollYawDegrees( float pitch , float yaw , float roll )
{
	m_rotationPitchRollYawDegrees.x = pitch;
	m_rotationPitchRollYawDegrees.y = yaw;
	m_rotationPitchRollYawDegrees.z = roll;

	/*m_rotationPitchRollYawDegrees.x = Clamp( m_rotationPitchRollYawDegrees.x , -90.f , 90.f );
	m_rotationPitchRollYawDegrees.y = Clamp( m_rotationPitchRollYawDegrees.y , -180.f , 180.f );
	m_rotationPitchRollYawDegrees.z = Clamp( m_rotationPitchRollYawDegrees.z , -180.f , 180.f );*/
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
