#include "Engine/Input/AnalogJoyStick.hpp"
#include "Engine/Math/MathUtils.hpp"

AnalogJoyStick::AnalogJoyStick( float innerDeadZoneFraction, float outerDeadZoneFraction ):m_innerDeadZoneFraction(innerDeadZoneFraction),m_outerDeadZoneFraction(outerDeadZoneFraction)
{

}

void AnalogJoyStick::Reset()
{
	m_RawPosition = Vec2( 0.f, 0.f );
    m_CorrectedPosition = Vec2( 0.f, 0.f );
	m_CorrectedDegrees = 0.f;
	m_CorrectedMagnitude = 0.f;
}

void AnalogJoyStick::UpdatePosition( float rawNormalizedX, float rawNormalizedY )
{
	m_RawPosition=Vec2(rawNormalizedX,rawNormalizedY);
	float rawMagnitude= m_RawPosition.GetLength();

	m_CorrectedDegrees=0.f;
	

	m_CorrectedMagnitude=ClampZeroToOne(RangeMapFloat(m_innerDeadZoneFraction,m_outerDeadZoneFraction,-1,1,rawMagnitude));

	if( m_CorrectedMagnitude>0.f )
	{
		m_CorrectedDegrees=m_RawPosition.GetAngleDegrees();
	}

	m_CorrectedPosition=Vec2::MakeFromPolarDegrees(m_CorrectedDegrees,m_CorrectedMagnitude);
	
}
