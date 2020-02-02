#pragma once

class Rigidbody2D;

class GameObject
{
public:
	
	Rigidbody2D* m_rigidbody = nullptr;
	GameObject() {};
	~GameObject();


};