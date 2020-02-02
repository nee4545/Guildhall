#include "Game/GameObject.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

GameObject::~GameObject()
{
	m_rigidbody->Destroy();
	m_rigidbody = nullptr;
}
