#pragma once
#include "Game/Entity.hpp"

class Texture;
class SpriteAnimDefinition;
class SpriteAnimDefTex;
class Game;

enum States
{
	CROUCH,
	STAND,
	WALK,
	CARRY_BARREL,
	CARRY_KNIFE,
};

struct AnimProperties
{
	SpriteAnimDefTex* animations;
	Vec2 aspect;
};

class GreenBeret :public Entity
{
public:
	GreenBeret(Game* game);
	~GreenBeret();

	virtual void Update( float deltaseconds ) override;
	virtual void Render() override;
	virtual void Die() override;

	void LoadAnimations();
	void LoadCrouchAnimations();
	void LoadWalkAnimations();
	void SetTexture();
	void ResetVertices();
	void ResetAndFlipVertices();

	AnimProperties* m_crouchR = nullptr;
	AnimProperties* m_crouchR1 = nullptr;
	AnimProperties* m_crouchR2 = nullptr;
	AnimProperties* m_crouchR3 = nullptr;
	AnimProperties* m_crouchB = nullptr;
	AnimProperties* m_crouchT = nullptr;
	AnimProperties* m_crouchT1 = nullptr;
	AnimProperties* m_crouchT2 = nullptr;
	AnimProperties* m_crouchT3 = nullptr;

	AnimProperties* m_walkR = nullptr;
	AnimProperties* m_walkR1 = nullptr;
	AnimProperties* m_walkB = nullptr;
	AnimProperties* m_walkT = nullptr;
	AnimProperties* m_walkT1 = nullptr;

	AnimProperties* m_knifeR = nullptr;
	AnimProperties* m_knifeB = nullptr;
	//AnimProperties* m_knife
	

	Game* m_game = nullptr;

	States m_state = WALK;

	Vertex_PCU m_vertices[ 6 ];
	Vec2 m_minUV = Vec2(0.f,0.f);
	Vec2 m_maxUV = Vec2(1.f,1.f);

	Vec2  aspect = Vec2( 0.5f , 0.5f );

	const Texture* m_currentTex = nullptr;

	float m_time = 0.f;

	Vec2 m_nextMovePosition;
	Vec2 m_cachedNextPosition;
	bool m_moveSet = false;
	bool potentialFieldQueryDone = false;
	bool shoudlGetFlippedPotentialFieldValues = false;
	bool shoudCachePosition = false;

};