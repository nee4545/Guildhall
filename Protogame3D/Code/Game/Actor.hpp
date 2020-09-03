#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Transform.hpp"

class Actor :public Entity
{
public:
	Actor( Vec2 position );
	Actor( EntityDef const& entityDef , Map* map );
	Actor();
	~Actor();

	AABB2 m_bounds;
	Transform m_transform;
	Mat44 m_tranformMatrix;
	
	Vec2 m_minUVs;
	Vec2 m_maxUVs;
	float m_time = 0.f;
	EntityDef* m_def = nullptr;

	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void DebugRender() override;
	virtual void Die() override;

	void UpdateSprites();

};