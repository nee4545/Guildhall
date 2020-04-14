#include"constantbuffers.hlsl"

float RangeMap( float val , float inMin , float inMax , float outMin , float outMax )
{
	float domain = inMax - inMin;
	float range = outMax - outMin;
	return ( ( val - inMin ) / domain ) * range + outMin;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
float4 ConvertNormalizedVector3ToColor( float3 vec3 )
{
    float4 color;
    color.x = RangeMap( vec3.x , -1.f , 1.f , 0.f , 1.f );
    color.y = RangeMap( vec3.y , -1.f , 1.f , 0.f , 1.f );
    color.z = RangeMap( vec3.z , -1.f , 1.f , 0.f , 1.f );
    color.w = 1;
    return color;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
float3 NormalColorToVector3( float3 color )
{
    return normalize( color * float3( 2.0f , 2.0f , 1.0f ) - float3( 1.0f , 1.0f , 0.0f ) );
}
//--------------------------------------------------------------------------------------------------------------------------------------------

float2 ComputeLightFactor(light_t light, float3 worldPos, float3 worldNormal, float3 dirToEye)
{
	float3 vecToLight = light.world_position - worldPos;

	float dist = length(vecToLight);
	float3 dirToLight = normalize(vecToLight);
	float3 attVec = float3(1.0f, dist, dist * dist);

	float diffAtt = light.intensity / dot(attVec, light.attenuation);
	float specAtt = light.intensity / dot(attVec, light.specularAttunation);

   // compute diffuse
	float diffuse = max(0.0f, dot(dirToLight, worldNormal));
	
   // blinn-phong
	float3 hv = normalize(dirToLight + dirToEye);
	float specular = max(0.0f, dot(normalize(worldNormal), hv));

   // finalize specular
	specular = SPECULAR_FACTOR * pow(specular, SPECULAR_POWER);

	return float2(diffAtt * diffuse, specAtt * specular);
}