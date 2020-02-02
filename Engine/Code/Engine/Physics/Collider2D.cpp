#include "Engine/Physics/Collider2D.hpp"

void Collider2D::Destroy()
{
	m_isGarbage = true;
}
