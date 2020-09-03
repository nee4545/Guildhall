#include "Game/BillBoard.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"

void UpdateCameraFacingXY( Camera& camera , Mat44& transform , Vec3 position )
{
	Vec3 targetLocation = camera.m_transform.m_position;

	Vec3 forward = targetLocation - position;
	forward=forward.GetNormalized();

	if ( forward.GetLengthSquared() <= 0.001f )
	{
		transform = Mat44();
	}

	Vec3 left = CrossProduct3D( -forward , Vec3( 0.f , 0.f , 1.f )).GetNormalized();

	if ( left.GetLengthSquared() <= 0.001f )
	{
		left = CrossProduct3D( forward , Vec3( 0.f , -1.f , 0.f ) );
	}

	Vec3 up = Vec3( 0.f , 0.f , 1.f );
	forward.z = 0.f;
	left.z = 0.f;

	transform.SetBasisVectors3D( left , up , forward , position );
}

void UpdateCameraOpposedXY( Camera& camera , Mat44& transform , Vec3 position )
{
	Vec3 forward = -camera.m_transform.ToMatrix().GetIBasis3D();
	forward = forward.GetNormalized();
	forward.z = 0.f;

	Vec3 left( -forward.y , forward.x , 0.f );

	transform.SetBasisVectors3D( left , Vec3(0.f,0.f,1.f) , forward );
	transform.SetTranslation3D( position );
}

void UpdateCameraFacingXYZ( Camera& camera , Mat44& transform , Vec3 position )
{
	Vec3 forward = -camera.m_transform.ToMatrix().GetIBasis3D();
	forward = forward.GetNormalized();

	Vec3 left = -camera.m_transform.ToMatrix().GetJBasis3D();
	left = left.GetNormalized();

	Vec3 up = camera.m_transform.ToMatrix().GetKBasis3D();
	up = up.GetNormalized();

	transform.SetBasisVectors3D( left , up , forward );
	transform.SetTranslation3D( position );
}

void UpdateCameraOpposedXYZ( Camera& camera , Mat44& transform , Vec3 position )
{
	Vec3 targetLocation = camera.m_transform.m_position;

	Vec3 forward = targetLocation - position;
	forward = forward.GetNormalized();

	if ( forward.GetLengthSquared() <= 0.001f )
	{
		transform = Mat44();
	}

	Vec3 left = CrossProduct3D( -forward , Vec3( 0.f , 0.f , 1.f ) ).GetNormalized();

	if ( left.GetLengthSquared() <= 0.001f )
	{
		left = CrossProduct3D( forward , Vec3( 0.f , -1.f , 0.f ) );
	}

	Vec3 up = Vec3( 0.f , 0.f , 1.f );
	transform.SetBasisVectors3D( left , up , forward , position );
}
