#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"


Mat44::Mat44( float* sixteenValuesBasisMajor )
{
	Ix = sixteenValuesBasisMajor[0];
	Iy = sixteenValuesBasisMajor[1];
	Iz = sixteenValuesBasisMajor[2];
	Iw = sixteenValuesBasisMajor[3];

	Jx = sixteenValuesBasisMajor[4];
	Jy = sixteenValuesBasisMajor[5];
	Jz = sixteenValuesBasisMajor[6];
	Jw = sixteenValuesBasisMajor[7];

	Kx = sixteenValuesBasisMajor[8];
	Ky = sixteenValuesBasisMajor[9];
	Kz = sixteenValuesBasisMajor[10];
	Kw = sixteenValuesBasisMajor[11];

	Tx = sixteenValuesBasisMajor[12];
	Ty = sixteenValuesBasisMajor[13];
	Tz = sixteenValuesBasisMajor[14];
	Tw = sixteenValuesBasisMajor[15];
}

Mat44::Mat44( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;
	Iz = 0;
	Iw = 0;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;
	Jz =0;
	Jw =0;

	Kx = 0;
	Ky = 0;
	Kz = 1;
	Kw = 0;

	Tx = translation2D.x;
	Ty = translation2D.y;
	Tz = 0;
	Tw = 1;

}

Mat44::Mat44( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;
	Iw = 0;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;
	Jw =0;

	Kx =  kBasis3D.x;
	Ky =  kBasis3D.y;
	Kz =  kBasis3D.z;
	Kw =  0;

	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
	Tw = 1;
}

Mat44::Mat44( const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4 translationHomogeneous )
{
	Ix = iBasisHomogeneous.x;
	Iy = iBasisHomogeneous.y;
	Iz = iBasisHomogeneous.z;
	Iw = iBasisHomogeneous.w;

	Jx = jBasisHomogeneous.x;
	Jy = jBasisHomogeneous.y;
	Jz = jBasisHomogeneous.z;
	Jw = jBasisHomogeneous.w;

	Kx =  kBasisHomogeneous.x;
	Ky =  kBasisHomogeneous.y;
	Kz =  kBasisHomogeneous.z;
	Kw =  kBasisHomogeneous.w;

	Tx = translationHomogeneous.x;
	Ty = translationHomogeneous.y;
	Tz = translationHomogeneous.z;
	Tw = translationHomogeneous.w;
}

const Vec2 Mat44::TransformVector2D( const Vec2& quantity ) const
{
	return Vec2(Ix*quantity.x+Jx*quantity.y,Iy*quantity.x+Jy*quantity.y);
}

const Vec3 Mat44::TransformVector3D( const Vec3& quantity ) const
{
	return Vec3(Ix*quantity.x + Jx*quantity.y + Kx*quantity.z, Iy*quantity.x + Jy*quantity.y + Ky*quantity.z , Iz*quantity.x + Jz*quantity.y +Kz*quantity.z );
}

const Vec2 Mat44::TransformPosition2D( const Vec2& position ) const
{
	return Vec2(Ix*position.x+Jx*position.y+Tx,Iy*position.x+Jy*position.y+Ty);
}

const Vec3 Mat44::TransformPosition3D( const Vec3& position ) const
{
	return Vec3(Ix*position.x + Jx*position.y + Kx*position.z+Tx, Iy*position.x + Jy*position.y + Ky*position.z + Ty, Iz*position.x + Jz*position.y +Kz*position.z + Tz);
}

const Vec4 Mat44::TransformHomogeneousPoint3D( const Vec4& homogeneousPoint ) const
{
	float Px = Ix*homogeneousPoint.x + Jx* homogeneousPoint.y + Kx*homogeneousPoint.z + Tx*homogeneousPoint.w;
	float Py = Iy*homogeneousPoint.x + Jy* homogeneousPoint.y + Ky*homogeneousPoint.z + Ty*homogeneousPoint.w;
	float Pz = Iz*homogeneousPoint.x + Jz* homogeneousPoint.y + Kz*homogeneousPoint.z + Tz*homogeneousPoint.w;
	float Pw = Iw*homogeneousPoint.x + Jw* homogeneousPoint.y + Kw*homogeneousPoint.z + Tw*homogeneousPoint.w;

	return Vec4(Px,Py,Pz,Pw);
}

const Vec2 Mat44::GetIBasis2D() const
{
	return Vec2(Ix,Iy);
}

const Vec2 Mat44::GetJBasis2D() const
{
	return Vec2(Jx,Jy);
}

const Vec2 Mat44::GetTranslation2D() const
{
	return Vec2(Tx,Ty);
}

const Vec3 Mat44::GetIBasis3D() const
{
	return Vec3(Ix,Iy,Iz);
}

const Vec3 Mat44::GetJBasis3D() const
{
	return Vec3(Jx,Jy,Jz);
}

const Vec3 Mat44::GetKBasis3D() const
{
	return Vec3(Kx,Ky,Kz);
}

const Vec3 Mat44::GetTranslation3D() const
{
	return Vec3(Tx,Ty,Tz);
}

const Vec4 Mat44::GetIBasis4D() const
{
	return Vec4(Ix,Iy,Iz,Iw);
}

const Vec4 Mat44::GetJBasis4D() const
{
	return Vec4(Jx,Jy,Jz,Jw);
}

const Vec4 Mat44::GetKBasis4D() const
{
	return Vec4(Kx,Ky,Kz,Kw);
}

const Vec4 Mat44::GetTranslation4D() const
{
	return Vec4(Tx,Ty,Tz,Tw);
}

void Mat44::SetTranslation2D( const Vec2& translation2D )
{
	Tx = translation2D.x;
	Ty = translation2D.y;
}

void Mat44::SetTranslation3D( const Vec3& translation3D )
{
	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

void Mat44::SetBasisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;
}

void Mat44::SetBaisisVectors2D( const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D )
{
	Ix = iBasis2D.x;
	Iy = iBasis2D.y;

	Jx = jBasis2D.x;
	Jy = jBasis2D.y;

	Tx = translation2D.x;
	Ty = translation2D.y;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;
}

void Mat44::SetBasisVectors3D( const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D )
{
	Ix = iBasis3D.x;
	Iy = iBasis3D.y;
	Iz = iBasis3D.z;

	Jx = jBasis3D.x;
	Jy = jBasis3D.y;
	Jz = jBasis3D.z;

	Kx = kBasis3D.x;
	Ky = kBasis3D.y;
	Kz = kBasis3D.z;

	Tx = translation3D.x;
	Ty = translation3D.y;
	Tz = translation3D.z;
}

void Mat44::SetBasisVectors4D( const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D )
{
	Ix = iBasis4D.x;
	Iy = iBasis4D.y;
	Iz = iBasis4D.z;
	Iw = iBasis4D.w;

	Jx = jBasis4D.x;
	Jy = jBasis4D.y;
	Jz = jBasis4D.z;
	Jw = jBasis4D.w;

	Kx = kBasis4D.x;
	Ky = kBasis4D.y;
	Kz = kBasis4D.z;
	Kw = kBasis4D.w;

	Tx = translation4D.x;
	Ty = translation4D.y;
	Tz = translation4D.z;
	Tw = translation4D.w;
}

const Mat44 Mat44::CreateXRotationDegrees( float degreesAboutX )
{
	Mat44 matrix;
	float c = CosDegrees(degreesAboutX);
	float s =SinDegrees(degreesAboutX);

	matrix.Jy = c;
	matrix.Jz = s;

	matrix.Ky = -s;
	matrix.Kz = c;

	return matrix;

}

const Mat44 Mat44::CreateYRotationDegrees( float degreesAboutY )
{
	Mat44 matrix;
	float c = CosDegrees( degreesAboutY );
	float s =SinDegrees( degreesAboutY );

	matrix.Ix = c;
	matrix.Iz = -s;

	matrix.Kx = s;
	matrix.Kz = c;

	return matrix;
}

const Mat44 Mat44::CreateZRotationDegrees( float degreesAboutZ )
{
	Mat44 matrix;
	float c = CosDegrees( degreesAboutZ );
	float s = SinDegrees( degreesAboutZ );

	matrix.Ix = c;
	matrix.Iy = s;

	matrix.Jx = -s;
	matrix.Jy = c;

	return matrix;
}

const Mat44 Mat44::CreateTranslationXY( const Vec2& translationXY )
{
	Mat44 matrix;

	matrix.Tx = translationXY.x;
	matrix.Ty = translationXY.y;

	return matrix;
}

const Mat44 Mat44::CreateTranslation3D( const Vec3& translation3D )
{
	Mat44 matrix;

	matrix.Tx = translation3D.x;
	matrix.Ty = translation3D.y;
	matrix.Tz = translation3D.z;

	return matrix;
}

const Mat44 Mat44::CreateUniformScaleXY( float uniformScaleXY )
{
	Mat44 matrix;

	matrix.Ix = uniformScaleXY;
	matrix.Jy = uniformScaleXY;

	return matrix;
}

const Mat44 Mat44::CreateNonUniformScaleXY( const Vec2& scaleFactorsXY )
{
	Mat44 matrix;

	matrix.Ix = scaleFactorsXY.x;
	matrix.Jy = scaleFactorsXY.y;

	return matrix;
}

const Mat44 Mat44::CreateUniformScale3D( float uniformScale3D )
{
	Mat44 matrix;

	matrix.Ix = uniformScale3D;
	matrix.Jy = uniformScale3D;
	matrix.Kz = uniformScale3D;

	return matrix;
}

const Mat44 Mat44::CreateNonUniformScale3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 matrix;

	matrix.Ix = scaleFactorsXYZ.x;
	matrix.Jy = scaleFactorsXYZ.y;
	matrix.Kz = scaleFactorsXYZ.z;

	return matrix;
}

const Mat44 Mat44::CreateOrthographicProjection( const Vec3& min , const Vec3& max )
{
	//think of x
	//min.x,max.x->(-1,1)

	//ndc.x = x / ( max.x - min.x ) - ( min.x / ( max.x - min.x ) ) * 2.0f + 1.0f
	//a= 1/(max.x-min.x)
	//b = (-2.f*min.x- max.x+min.x)/(max.x-min.x)

	Vec3 diff = max - min;
	Vec3 sum = max + min;

	float mat[] = { 2.0f / diff.x ,    0.f,         0.f,             0.f,
					0.f,               2.0f / diff.y, 0.f,             0.f,
					0.f,               0.f,         1.0f / diff.z,     0.f,
					-sum.x / diff.x,    -sum.y / diff.y,-min.z / diff.z,   1.f };

	return Mat44( mat );

}

const Mat44 Mat44::CreateFromScaleRotationPosition( const Vec3& scale , float pitch, float yaw, float roll , const Vec3& position, Convensions convention /*= CONVENTSION_DEFAULT*/ )
{
	Mat44 transform;
	if ( convention == CONVENTION_XRIGHT_YUP_ZBACK )
	{
		Mat44 scaleMatrix = CreateNonUniformScale3D( scale );
		Mat44 tranformPitch = CreateXRotationDegrees( pitch );
		Mat44 tranformYaw = CreateYRotationDegrees( yaw );
		Mat44 tranformRoll = CreateZRotationDegrees( roll );
		Mat44 translateBy = CreateTranslation3D( position );

		//--------------------------------------------------------------------------------------------------------------------------------------------
		transform.TransformBy( translateBy );
		transform.TransformBy( tranformRoll );
		transform.TransformBy( tranformYaw );
		transform.TransformBy( tranformPitch );
		transform.TransformBy( scaleMatrix );
	}

	if ( convention == CONVENSION_XEAST_YNORTH_ZUP )
	{
		Mat44 scaleMatrix = CreateNonUniformScale3D( scale );
		Mat44 tranformPitch = CreateYRotationDegrees( pitch );
		Mat44 tranformYaw = CreateZRotationDegrees( yaw );
		Mat44 tranformRoll = CreateXRotationDegrees( roll );
		Mat44 translateBy = CreateTranslation3D( position );

		//--------------------------------------------------------------------------------------------------------------------------------------------
		transform.TransformBy( translateBy );
		transform.TransformBy( tranformRoll );
		transform.TransformBy( tranformYaw );
		transform.TransformBy( tranformPitch );
		transform.TransformBy( scaleMatrix );
	}

	return transform;
}



const Mat44 Mat44::CreatePerspectiveProjection( float fovDegrees , float aspectRatio , float nearZ , float farZ )
{

	float height = 1.0f / (SinDegrees( fovDegrees * .5f )/CosDegrees( fovDegrees * .5f )); // how far away are we for the perspective point to be "one up" from our forward line. 
	float zrange = farZ - nearZ;
	float q = 1.0f / zrange;

	float proj[] = 
	{
		height / aspectRatio , 0.f , 0.f , 0.f ,
		0.f , height , 0.f , 0.f ,
		0.f , 0.f , -farZ * q , -1.f ,
		0.f , 0.f , nearZ * farZ * q , 0.f
	};
	

	return Mat44( proj );
}

void Mat44::MatrixTranspose( Mat44& mat )
{
	Mat44 temp;
	//temp.SetBasisVectors3D( mat.GetIBasis3D() , mat.GetJBasis3D() , mat.GetKBasis3D() );

	temp.Ix = mat.Ix;
	temp.Iy = mat.Jx;
	temp.Iz = mat.Kx;
	temp.Iw = mat.Tx;

	temp.Jx = mat.Iy;
	temp.Jy = mat.Jy;
	temp.Jz = mat.Ky;
	temp.Jw = mat.Ty;

	temp.Kx = mat.Iz;
	temp.Ky = mat.Jz;
	temp.Kz = mat.Kz;
	temp.Kw = mat.Tz;

	temp.Tx = mat.Iw;
	temp.Ty = mat.Jw;
	temp.Tz = mat.Kw;
	temp.Tw = mat.Tw;

	mat = temp;
}

void Mat44::MatrixInvertOrthoNormal( Mat44& mat )
{
	Mat44 inverse = mat;
	inverse.SetTranslation3D( Vec3(0.f,0.f,0.f) );
	MatrixTranspose( inverse );
	Vec3 translation = mat.GetTranslation3D();
	Vec3 inverseTranslation = inverse.TransformPosition3D( -translation );
	inverse.SetTranslation3D( inverseTranslation );
	mat = inverse;
}

void Mat44::MatrixInvert( Mat44& mat )
{
	float inverse[ 16 ];
	float determinant;
	float m[ 16 ];
	unsigned int index;
	for ( index = 0; index < 16; ++index )
	{
		m[ index ] = *( &mat.Ix + index );
	}
	inverse[ 0 ] = m[ 5 ] * m[ 10 ] * m[ 15 ] -
		m[ 5 ] * m[ 11 ] * m[ 14 ] -
		m[ 9 ] * m[ 6 ] * m[ 15 ] +
		m[ 9 ] * m[ 7 ] * m[ 14 ] +
		m[ 13 ] * m[ 6 ] * m[ 11 ] -
		m[ 13 ] * m[ 7 ] * m[ 10 ];
	inverse[ 4 ] = -m[ 4 ] * m[ 10 ] * m[ 15 ] +
		m[ 4 ] * m[ 11 ] * m[ 14 ] +
		m[ 8 ] * m[ 6 ] * m[ 15 ] -
		m[ 8 ] * m[ 7 ] * m[ 14 ] -
		m[ 12 ] * m[ 6 ] * m[ 11 ] +
		m[ 12 ] * m[ 7 ] * m[ 10 ];
	inverse[ 8 ] = m[ 4 ] * m[ 9 ] * m[ 15 ] -
		m[ 4 ] * m[ 11 ] * m[ 13 ] -
		m[ 8 ] * m[ 5 ] * m[ 15 ] +
		m[ 8 ] * m[ 7 ] * m[ 13 ] +
		m[ 12 ] * m[ 5 ] * m[ 11 ] -
		m[ 12 ] * m[ 7 ] * m[ 9 ];
	inverse[ 12 ] = -m[ 4 ] * m[ 9 ] * m[ 14 ] +
		m[ 4 ] * m[ 10 ] * m[ 13 ] +
		m[ 8 ] * m[ 5 ] * m[ 14 ] -
		m[ 8 ] * m[ 6 ] * m[ 13 ] -
		m[ 12 ] * m[ 5 ] * m[ 10 ] +
		m[ 12 ] * m[ 6 ] * m[ 9 ];
	inverse[ 1 ] = -m[ 1 ] * m[ 10 ] * m[ 15 ] +
		m[ 1 ] * m[ 11 ] * m[ 14 ] +
		m[ 9 ] * m[ 2 ] * m[ 15 ] -
		m[ 9 ] * m[ 3 ] * m[ 14 ] -
		m[ 13 ] * m[ 2 ] * m[ 11 ] +
		m[ 13 ] * m[ 3 ] * m[ 10 ];
	inverse[ 5 ] = m[ 0 ] * m[ 10 ] * m[ 15 ] -
		m[ 0 ] * m[ 11 ] * m[ 14 ] -
		m[ 8 ] * m[ 2 ] * m[ 15 ] +
		m[ 8 ] * m[ 3 ] * m[ 14 ] +
		m[ 12 ] * m[ 2 ] * m[ 11 ] -
		m[ 12 ] * m[ 3 ] * m[ 10 ];
	inverse[ 9 ] = -m[ 0 ] * m[ 9 ] * m[ 15 ] +
		m[ 0 ] * m[ 11 ] * m[ 13 ] +
		m[ 8 ] * m[ 1 ] * m[ 15 ] -
		m[ 8 ] * m[ 3 ] * m[ 13 ] -
		m[ 12 ] * m[ 1 ] * m[ 11 ] +
		m[ 12 ] * m[ 3 ] * m[ 9 ];
	inverse[ 13 ] = m[ 0 ] * m[ 9 ] * m[ 14 ] -
		m[ 0 ] * m[ 10 ] * m[ 13 ] -
		m[ 8 ] * m[ 1 ] * m[ 14 ] +
		m[ 8 ] * m[ 2 ] * m[ 13 ] +
		m[ 12 ] * m[ 1 ] * m[ 10 ] -
		m[ 12 ] * m[ 2 ] * m[ 9 ];
	inverse[ 2 ] = m[ 1 ] * m[ 6 ] * m[ 15 ] -
		m[ 1 ] * m[ 7 ] * m[ 14 ] -
		m[ 5 ] * m[ 2 ] * m[ 15 ] +
		m[ 5 ] * m[ 3 ] * m[ 14 ] +
		m[ 13 ] * m[ 2 ] * m[ 7 ] -
		m[ 13 ] * m[ 3 ] * m[ 6 ];
	inverse[ 6 ] = -m[ 0 ] * m[ 6 ] * m[ 15 ] +
		m[ 0 ] * m[ 7 ] * m[ 14 ] +
		m[ 4 ] * m[ 2 ] * m[ 15 ] -
		m[ 4 ] * m[ 3 ] * m[ 14 ] -
		m[ 12 ] * m[ 2 ] * m[ 7 ] +
		m[ 12 ] * m[ 3 ] * m[ 6 ];
	inverse[ 10 ] = m[ 0 ] * m[ 5 ] * m[ 15 ] -
		m[ 0 ] * m[ 7 ] * m[ 13 ] -
		m[ 4 ] * m[ 1 ] * m[ 15 ] +
		m[ 4 ] * m[ 3 ] * m[ 13 ] +
		m[ 12 ] * m[ 1 ] * m[ 7 ] -
		m[ 12 ] * m[ 3 ] * m[ 5 ];
	inverse[ 14 ] = -m[ 0 ] * m[ 5 ] * m[ 14 ] +
		m[ 0 ] * m[ 6 ] * m[ 13 ] +
		m[ 4 ] * m[ 1 ] * m[ 14 ] -
		m[ 4 ] * m[ 2 ] * m[ 13 ] -
		m[ 12 ] * m[ 1 ] * m[ 6 ] +
		m[ 12 ] * m[ 2 ] * m[ 5 ];
	inverse[ 3 ] = -m[ 1 ] * m[ 6 ] * m[ 11 ] +
		m[ 1 ] * m[ 7 ] * m[ 10 ] +
		m[ 5 ] * m[ 2 ] * m[ 11 ] -
		m[ 5 ] * m[ 3 ] * m[ 10 ] -
		m[ 9 ] * m[ 2 ] * m[ 7 ] +
		m[ 9 ] * m[ 3 ] * m[ 6 ];
	inverse[ 7 ] = m[ 0 ] * m[ 6 ] * m[ 11 ] -
		m[ 0 ] * m[ 7 ] * m[ 10 ] -
		m[ 4 ] * m[ 2 ] * m[ 11 ] +
		m[ 4 ] * m[ 3 ] * m[ 10 ] +
		m[ 8 ] * m[ 2 ] * m[ 7 ] -
		m[ 8 ] * m[ 3 ] * m[ 6 ];
	inverse[ 11 ] = -m[ 0 ] * m[ 5 ] * m[ 11 ] +
		m[ 0 ] * m[ 7 ] * m[ 9 ] +
		m[ 4 ] * m[ 1 ] * m[ 11 ] -
		m[ 4 ] * m[ 3 ] * m[ 9 ] -
		m[ 8 ] * m[ 1 ] * m[ 7 ] +
		m[ 8 ] * m[ 3 ] * m[ 5 ];
	inverse[ 15 ] = m[ 0 ] * m[ 5 ] * m[ 10 ] -
		m[ 0 ] * m[ 6 ] * m[ 9 ] -
		m[ 4 ] * m[ 1 ] * m[ 10 ] +
		m[ 4 ] * m[ 2 ] * m[ 9 ] +
		m[ 8 ] * m[ 1 ] * m[ 6 ] -
		m[ 8 ] * m[ 2 ] * m[ 5 ];
	determinant = m[ 0 ] * inverse[ 0 ] + m[ 1 ] * inverse[ 4 ] + m[ 2 ] * inverse[ 8 ] + m[ 3 ] * inverse[ 12 ];
	determinant = 1.0f / determinant;
	Mat44 ret( inverse );
	for ( index = 0; index < 16; index++ )
	{
		*( &ret.Ix + index ) = ( inverse[ index ] * determinant );
	}
	mat = ret;
}

Mat44 Mat44::LookAt( Vec3 pos , Vec3 target , Vec3 worldUp /*= Vec3(0.f,1.f,0.f)*/ )
{
	Vec3 forward = ( target - pos ).GetNormalized();

	if ( forward.GetLengthSquared() <= 0.001 )
	{
		return Mat44();
	}

	Vec3 right = CrossProduct3D( forward , worldUp ).GetNormalized();
	Vec3 v = Vec3( 0.f , 0.f , -1.f );

	if ( right.GetLengthSquared() <= 0.001 )
	{
		right = CrossProduct3D( forward , v);
	}

	Vec3 up = CrossProduct3D( right , forward );
	Mat44 lookAt;
	lookAt.SetBasisVectors3D( right , up , -forward , pos );

	return lookAt;
}

void Mat44::RotateXDegrees( float degreesAboutX )
{
	Mat44 rotationMatrix = CreateXRotationDegrees(degreesAboutX);
	TransformBy(rotationMatrix);
}

void Mat44::RotateYDegrees( float degreesAboutY )
{
	Mat44 rotationMatrix = CreateYRotationDegrees( degreesAboutY );
	TransformBy( rotationMatrix );
}

void Mat44::RotateZDegrees( float degreesAboutZ )
{
	Mat44 rotationMatrix = CreateZRotationDegrees( degreesAboutZ );
	TransformBy( rotationMatrix );
}

void Mat44::Translate2D( const Vec2& translationXY )
{
	Mat44 translationMatrix = CreateTranslationXY(translationXY);
	TransformBy(translationMatrix);
}

void Mat44::Translate3D( const Vec3& translation3D )
{
	Mat44 translationMatrix = CreateTranslation3D(translation3D);
	TransformBy(translationMatrix);
}

void Mat44::ScaleUniform2D( float uniformScaleXY )
{
	Mat44 scaleMatrix = CreateUniformScaleXY(uniformScaleXY);
	TransformBy(scaleMatrix);
}

void Mat44::ScaleNonUniform2D( const Vec2& scaleFactorsXY )
{
	Mat44 scaleMatrix = CreateNonUniformScaleXY(scaleFactorsXY);
	TransformBy(scaleMatrix);
}

void Mat44::ScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 scaleMatrix = CreateUniformScale3D(uniformScaleXYZ);
	TransformBy(scaleMatrix);
}

void Mat44::ScaleNonUniform3D( const Vec3& scaleFactorsXYZ )
{
	Mat44 scaleMatrix = CreateNonUniformScale3D(scaleFactorsXYZ);
	TransformBy(scaleMatrix);

}

void Mat44::TransformBy( const Mat44& arbitaryTransformation )
{
	Mat44 matrixCopy = *this;

	Ix = matrixCopy.Ix* arbitaryTransformation.Ix + matrixCopy.Jx * arbitaryTransformation.Iy + matrixCopy.Kx*arbitaryTransformation.Iz + matrixCopy.Tx*arbitaryTransformation.Iw;
	Iy = matrixCopy.Iy* arbitaryTransformation.Ix + matrixCopy.Jy * arbitaryTransformation.Iy + matrixCopy.Ky*arbitaryTransformation.Iz + matrixCopy.Ty*arbitaryTransformation.Iw;
	Iz = matrixCopy.Iz* arbitaryTransformation.Ix + matrixCopy.Jz * arbitaryTransformation.Iy + matrixCopy.Kz*arbitaryTransformation.Iz + matrixCopy.Tz*arbitaryTransformation.Iw;
	Iw = matrixCopy.Iw* arbitaryTransformation.Ix + matrixCopy.Jw * arbitaryTransformation.Iy + matrixCopy.Kw*arbitaryTransformation.Iz + matrixCopy.Tw*arbitaryTransformation.Iw;

	Jx = matrixCopy.Ix* arbitaryTransformation.Jx + matrixCopy.Jx * arbitaryTransformation.Jy + matrixCopy.Kx*arbitaryTransformation.Jz + matrixCopy.Tx*arbitaryTransformation.Jw;
	Jy = matrixCopy.Iy* arbitaryTransformation.Jx + matrixCopy.Jy * arbitaryTransformation.Jy + matrixCopy.Ky*arbitaryTransformation.Jz + matrixCopy.Ty*arbitaryTransformation.Jw;
	Jz = matrixCopy.Iz* arbitaryTransformation.Jx + matrixCopy.Jz * arbitaryTransformation.Jy + matrixCopy.Kz*arbitaryTransformation.Jz + matrixCopy.Tz*arbitaryTransformation.Jw;
	Jw = matrixCopy.Iw* arbitaryTransformation.Jx + matrixCopy.Jw * arbitaryTransformation.Jy + matrixCopy.Kw*arbitaryTransformation.Jz + matrixCopy.Tw*arbitaryTransformation.Jw;

	Kx = matrixCopy.Ix* arbitaryTransformation.Kx + matrixCopy.Jx * arbitaryTransformation.Ky + matrixCopy.Kx*arbitaryTransformation.Kz + matrixCopy.Tx*arbitaryTransformation.Kw;
	Ky = matrixCopy.Iy* arbitaryTransformation.Kx + matrixCopy.Jy * arbitaryTransformation.Ky + matrixCopy.Ky*arbitaryTransformation.Kz + matrixCopy.Ty*arbitaryTransformation.Kw;
	Kz = matrixCopy.Iz* arbitaryTransformation.Kx + matrixCopy.Jz * arbitaryTransformation.Ky + matrixCopy.Kz*arbitaryTransformation.Kz + matrixCopy.Tz*arbitaryTransformation.Kw;
	Kw = matrixCopy.Iw* arbitaryTransformation.Kx + matrixCopy.Jw * arbitaryTransformation.Ky + matrixCopy.Kw*arbitaryTransformation.Kz + matrixCopy.Tw*arbitaryTransformation.Kw;

	Tx = matrixCopy.Ix* arbitaryTransformation.Tx + matrixCopy.Jx * arbitaryTransformation.Ty + matrixCopy.Kx*arbitaryTransformation.Tz + matrixCopy.Tx*arbitaryTransformation.Tw;
	Ty = matrixCopy.Iy* arbitaryTransformation.Tx + matrixCopy.Jy * arbitaryTransformation.Ty + matrixCopy.Ky*arbitaryTransformation.Tz + matrixCopy.Ty*arbitaryTransformation.Tw;
	Tz = matrixCopy.Iz* arbitaryTransformation.Tx + matrixCopy.Jz * arbitaryTransformation.Ty + matrixCopy.Kz*arbitaryTransformation.Tz + matrixCopy.Tz*arbitaryTransformation.Tw;
	Tw = matrixCopy.Iw* arbitaryTransformation.Tx + matrixCopy.Jw * arbitaryTransformation.Ty + matrixCopy.Kw*arbitaryTransformation.Tz + matrixCopy.Tw*arbitaryTransformation.Tw;
}

