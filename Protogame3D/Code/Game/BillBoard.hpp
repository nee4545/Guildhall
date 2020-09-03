#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"

class Camera;


void UpdateCameraFacingXY(Camera& camera,Mat44& transform, Vec3 position);
void UpdateCameraOpposedXY( Camera& camera , Mat44& transform , Vec3 position );
void UpdateCameraFacingXYZ( Camera& camera , Mat44& transform , Vec3 position );
void UpdateCameraOpposedXYZ( Camera& camera , Mat44& transform , Vec3 position );
