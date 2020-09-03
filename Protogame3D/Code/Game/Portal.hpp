#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/AABB2.hpp"
#include "Engine/Core/Transform.hpp"

class Portal : public Entity
{
public:
	Portal( EntityDef const& entityDef , Map* map );
	~Portal();

	std::map<std::string , SpriteAnimDefinition*> m_animations;

	std::string m_destMap;
	Vec2 m_desPos;
	float m_desYawOffset = 0.f;

	Vec2 m_minUVs;
	Vec2 m_maxUVs;

	float m_time = 0.f;

	AABB2 m_bounds;
	Transform m_transform;
	Mat44 m_tranformMatrix;

	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void DebugRender() override;

	void UpdateSprites();
};
