
static const uint MAX_LIGHTS = 1;

struct light_t
{
	float3 world_position;
	float pad00; // this is not required, but know the GPU will add this padding to make the next variable 16-byte aligned
	
    float3 direction;
    float directionFactor;

	float3 color;
	float intensity; // rgb and an intensity
	
	float3 attenuation;
	float innerAngle;
	
	float3 specularAttunation;
	float dotOuterAngle;
};

cbuffer time_constants:register( b0 )
{
	float SYSTEM_TIME_SECONDS;
	float SYSTEM_TIME_DELTASECONDS;
	
	float padding;
	float INVERSE_GAMMA;
};

cbuffer camera_constants: register( b1 )
{
	float4x4 PROJECTION; //CAMERA_TO_CLIP_TRANSFORM
	float4x4 VIEW;
	
	float3 CAMERA_POSITION;
	float PADDING;
};

cbuffer model_constants: register( b2 )
{
	float4x4 MODEL;
};

cbuffer light_constants : register( b3 )         // constant buffer slot 3
{
	float4 AMBIENT;
	light_t LIGHT[MAX_LIGHTS];
	
	// all 0 to 1 and help influence the lighting equation
	float DIFFUSE_FACTOR; // default: 1  - scales diffuse lighting in equation (lower values make an object absorb light
	float SPECULAR_FACTOR; // default: 1  - scales specular lighting in the equation (higher values create a "shinier" surface)
	float SPECULAR_POWER; // default: 32 - controls the specular falloff (higher values create a "smoother" looking surface)
	float EMISSIVE_FACTOR; // default: 1  - controls how much the emissive texture is added into the final equation
};

Texture2D <float4> tDiffuse : register( t0 );
Texture2D <float4> tNormal : register( t1 );
SamplerState sSampler : register( s0 );
