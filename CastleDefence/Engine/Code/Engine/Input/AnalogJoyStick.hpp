#pragma once
#include "Engine/Math/Vec2.hpp"

class AnalogJoyStick
{
	friend class XboxController;

public:

	explicit AnalogJoyStick(float innerDeadZoneFraction, float outerDeadZoneFraction);

	Vec2 GetPostion() const {return m_CorrectedPosition;}
	float GetMagnitude() const {return m_CorrectedMagnitude;}
	float GetAngleDegrees() const {return m_CorrectedDegrees;}
	Vec2 GetRawPosition() const{return m_RawPosition;}
	float GetInnerDeadZoneFraction() const{return m_innerDeadZoneFraction;}
	float GetOuterDeadZoneFraction() const{return m_outerDeadZoneFraction;}
	void Reset();


private:

	void UpdatePosition(float rawNormalizedX, float rawNormalizedY);

	const float m_innerDeadZoneFraction;
	const float m_outerDeadZoneFraction;
	Vec2 m_RawPosition = Vec2(0.f,0.f);
	Vec2 m_CorrectedPosition = Vec2(0.f,0.f);
	float m_CorrectedDegrees = 0.f;
	float m_CorrectedMagnitude = 0.f;


};