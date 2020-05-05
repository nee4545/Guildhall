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
    //return dist.xx;
    //dirToLight = -dirToLight;
	
    float3 lightDir = lerp(-dirToLight, light.direction, light.directionFactor);
    float3 attVec = float3(1.f, dist, dist * dist);
    
    float dotAngle = dot(-dirToLight, light.direction);
    float coneAtt = smoothstep(light.dotOuterAngle, light.innerAngle, dotAngle);
    //return float2(dotAngle.xx);
    
    float diffAtt = light.intensity / dot(attVec, light.attenuation) * coneAtt;
    float specAtt = light.intensity / dot(attVec, light.specularAttunation) * coneAtt;
    
    
    float dotIncident = dot(-lightDir, worldNormal);
    float facing = smoothstep(-0.4f, 0.1f, dotIncident);
    float diffuse = max(0.f, dotIncident);
    
    float3 hv = normalize(-lightDir + dirToEye);
    float specular = max(0.f, dot(worldNormal, hv));
    
    specular = facing * specular;
    specular = SPECULAR_FACTOR*pow(specular, SPECULAR_POWER);
    
    return float2(diffAtt * diffuse, specAtt * specular);
    
}

struct lightRes
{
    float3 diffuseRes;
    float3 specRes;
};

lightRes ComputeLightingAt(float3 worldPos, float3 worldNormal, float3 surfaceColor, float specFactor)
{
    float3 dirToEye = normalize(CAMERA_POSITION - worldPos);
	
    float3 diffuse = AMBIENT.xyz * AMBIENT.w;
  
    float3 spec = float3(0.f.xxx);
	
    for (uint i = 0; i < MAX_LIGHTS; i++)
    {
       float3 lightColor = LIGHT[i].color.xyz;
       float2 lightFactors = ComputeLightFactor(LIGHT[i], worldPos, worldNormal, dirToEye);
       
       diffuse += lightFactors.x * lightColor;
       spec += lightFactors.y * lightColor;
    }
	
    diffuse = min(diffuse, float3(1.f.xxx));
	
    lightRes result;
    
    result.diffuseRes = (diffuse * surfaceColor) + spec;
    result.specRes = spec;
    return result;
}