#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/EngineCommon.hpp"

struct Mat44
{
	float Ix=1;
	float Iy=0;
	float Iz=0;
	float Iw=0;

	float Jx=0;
	float Jy=1;
	float Jz=0;
	float Jw=0;

	float Kx=0;
	float Ky=0;
	float Kz=1;
	float Kw=0;

	float Tx=0;
	float Ty=0;
	float Tz=0;
	float Tw=1;

	const static Mat44 IDENTITY;

	Mat44()=default;
	explicit Mat44(float* sixteenValuesBasisMajor);
	explicit Mat44(const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D);
	explicit Mat44(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3&kBasis3D  ,const Vec3& translation3D);	
	explicit Mat44(const Vec4& iBasisHomogeneous, const Vec4& jBasisHomogeneous, const Vec4& kBasisHomogeneous, const Vec4 translationHomogeneous);

	//Transforming positions and vectors using this matrix
	const Vec2 TransformVector2D(const Vec2& quantity) const; //Assumes z=0,w=0
	const Vec3 TransformVector3D(const Vec3& quantity) const; //Assumes w=0
	const Vec2 TransformPosition2D(const Vec2& position) const; //Assumes w=1,z=0
	const Vec3 TransformPosition3D( const Vec3& position) const; //Assumes w=1
	const Vec4 TransformHomogeneousPoint3D( const Vec4& homogeneousPoint) const;

	//Basic Accessors

	const float* GetAsFloatArray() const { return &Ix; }
	float* GetAsFloatArray() {return &Ix;}
	const Vec2 GetIBasis2D() const;
	const Vec2 GetJBasis2D() const;
	const Vec2 GetTranslation2D() const;
	const Vec3 GetIBasis3D() const;
	const Vec3 GetJBasis3D() const;
	const Vec3 GetKBasis3D() const;
	const Vec3 GetTranslation3D() const;
	const Vec4 GetIBasis4D() const;
	const Vec4 GetJBasis4D() const;
	const Vec4 GetKBasis4D() const;
	const Vec4 GetTranslation4D() const;

	// Basic Mutators

	void SetTranslation2D(const Vec2& translation2D);
	void SetTranslation3D(const Vec3& translation3D);
	void SetBasisVectors2D(const Vec2& iBasis2D, const Vec2& jBasis2D);
	void SetBaisisVectors2D(const Vec2& iBasis2D, const Vec2& jBasis2D, const Vec2& translation2D);
	void SetBasisVectors3D(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D);
	void SetBasisVectors3D(const Vec3& iBasis3D, const Vec3& jBasis3D, const Vec3& kBasis3D, const Vec3& translation3D);
	void SetBasisVectors4D(const Vec4& iBasis4D, const Vec4& jBasis4D, const Vec4& kBasis4D, const Vec4& translation4D);

	

   //Static creation Methods

	static const Mat44 CreateXRotationDegrees(float degreesAboutX);
	static const Mat44 CreateYRotationDegrees(float degreesAboutY);
	static const Mat44 CreateZRotationDegrees(float degreesAboutZ);
	static const Mat44 CreateTranslationXY (const Vec2& translationXY);
	static const Mat44 CreateTranslation3D (const Vec3& translation3D);
	static const Mat44 CreateUniformScaleXY(float uniformScaleXY);
	static const Mat44 CreateNonUniformScaleXY(const Vec2& scaleFactorsXY);
	static const Mat44 CreateUniformScale3D(float uniformScale3D);
	static const Mat44 CreateNonUniformScale3D(const Vec3& scaleFactorsXYZ);

	static const Mat44 CreateFromScaleRotationPosition( const Vec3& scale , float pitch, float yaw, float roll , const Vec3& position , Convensions convention = CONVENTION_XRIGHT_YUP_ZBACK);

	//projection
	static const Mat44 CreateOrthographicProjection( const Vec3& min , const Vec3& max );
	static const Mat44 CreatePerspectiveProjection( float fovDegrees , float aspectRatio , float nearZ , float farZ );


	static void MatrixTranspose( Mat44& mat );          // A04
	static void MatrixInvertOrthoNormal( Mat44& mat );  // A04
	static void MatrixInvert( Mat44& mat );

	static Mat44 LookAt(Vec3 pos,Vec3 target, Vec3 worldUp = Vec3(0.f,1.f,0.f));

  //Transformation Mutators

	void RotateXDegrees(float degreesAboutX);
	void RotateYDegrees(float degreesAboutY);
	void RotateZDegrees(float degreesAboutZ);
	void Translate2D( const Vec2& translationXY);
	void Translate3D( const Vec3& translation3D);
	void ScaleUniform2D( float uniformScaleXY);
	void ScaleNonUniform2D( const Vec2& scaleFactorsXY);
	void ScaleUniform3D( float uniformScaleXYZ);
	void ScaleNonUniform3D(const Vec3& scaleFactorsXYZ);
	void TransformBy( const Mat44& arbitaryTransformation);

private:
	const Mat44 operator* (const Mat44& rhs) const =delete;

};


