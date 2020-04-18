#include "ShaderUtils.hlsl"

//--------------------------------------------------------------------------------------
// Stream Input
// ------
// Stream Input is input that is walked by the vertex shader.  
// If you say "Draw(3,0)", you are telling to the GPU to expect '3' sets, or 
// elements, of input data.  IE, 3 vertices.  Each call of the VertxShader
// we be processing a different element. 
//--------------------------------------------------------------------------------------

// Input from Vertex Buffers
struct vs_input_t
{
   // we are not defining our own input data; 
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
};

//--------------------------------------------------------------------------------------
// Programmable Shader Stages
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// for passing data from vertex to fragment (v-2-f)
struct v2f_t
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;


	float3 world_position : WORLD_POSITION;
	float3 world_normal : WORLD_NORMAL;
	float4 world_tangent : WORLD_TANGENT;
};
//--------------------------------------------------------------------------------------
//                          VERTEX SHADER
//--------------------------------------------------------------------------------------

v2f_t VertexFunction(vs_input_t input)
{
	v2f_t v2f			= (v2f_t) 0;

   // move the vertex through the spaces
	float4 local_pos	= float4(input.position, 1.0f); 
	float4 world_pos	= mul(MODEL, local_pos); 
	float4 camera_pos	= mul(VIEW, world_pos);
	float4 clip_pos		= mul(PROJECTION, camera_pos);

   // normal is currently in model/local space
	float4 local_normal  = float4(input.normal, 0.0f);
	float4 world_normal  = mul(MODEL, local_normal);

	float4 local_tangent = float4(input.tangent.xyz, 0.0f);
	float4 world_tangent = mul(MODEL, local_tangent);
	
   // tangent & bitangent

	v2f.position		= clip_pos; // we want to output the clip position to raster (a perspective point)
	v2f.color			= input.color;
	v2f.uv				= input.uv;
	v2f.world_position	= world_pos.xyz;
	v2f.world_normal	= world_normal.xyz;
    v2f.world_tangent = float4(world_tangent.xyz, input.tangent.w);
	return v2f;
}

//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//                  FRAGMENT SHADER
//--------------------------------------------------------------------------------------
// SV_Target0 at the end means the float4 being returned
// is being drawn to the first bound color target.
//--------------------------------------------------------------------------------------

float4 FragmentFunction(v2f_t input) : SV_Target0
{

    float4 diffuseColor = tDiffuse.Sample(sSampler, input.uv);
    
    float4 normalColor = tNormal.Sample(sSampler, input.uv);
    float3 dir_to_eye = normalize(CAMERA_POSITION - input.world_position);
    float3 tangent = normalize(input.world_tangent.xyz);
    float3 normal = normalize(input.world_normal);

    float3 bitangent = normalize(cross(normal, tangent)) * input.world_tangent.w;
    float3x3 TBN = float3x3(tangent, bitangent, normal);
	
    float alpha = diffuseColor.w * input.color.w;
	
    float3 surfaceColor = (input.color * diffuseColor).xyz;
	
    float3 surfaceNormal = NormalColorToVector3(normalColor.xyz);
    float3 worldNormal = mul(surfaceNormal, TBN);
	
    float3 finalColor = ComputeLightingAt(input.world_position, worldNormal, surfaceColor, 1.f);
    
	
    return float4(finalColor, alpha);
	
 //   float4 diffuseColor = tDiffuse.Sample(sSampler, input.uv);
 //   float4 normalColor = tNormal.Sample(sSampler, input.uv);
	
	////float INVERSE_GAMMA = 0.45454545f;
 //   float3 dirToEye = normalize(CAMERA_POSITION - input.world_position);
 //   float3 tangent = normalize(input.world_tangent.xyz);
 //   float3 normal = normalize(input.world_normal);

 //   float3 bitangent = normalize(cross(normal, tangent)) * input.world_tangent.w;
 //   float3x3 TBN = float3x3(tangent, bitangent, normal);

    //float3 surfaceColor = pow(diffuseColor.xyz, 0.46f);
    //surfaceColor = surfaceColor * input.color.xyz;
    //float3 surfaceColor = (input.color * diffuseColor).xyz;
    //float surfaceAlpha = input.color.w * diffuseColor.w;
	
    //float3 diffuse = AMBIENT.xyz * AMBIENT.w;
    
    //float3 surfaceNormal = NormalColorToVector3(normalColor.xyz);
    //surfaceNormal = normalize(input.world_normal);
    //float3 worldNormal = mul(surfaceNormal, TBN);
	
    //float3 light_position = LIGHT[0].world_position;
    //float3 dir_to_light = normalize(light_position - input.world_position);
    //float dot3 = max(0.0f, dot(dir_to_light, surfaceNormal));

    //diffuse += dot3;
    
    
    //float3 vecToLight = LIGHT[0].world_position - input.world_position;
    
    
    //float dist = length(vecToLight);
    //float3 dirToLight = normalize(vecToLight);
    
    //vecToLight = lerp(-dir_to_light, LIGHT[0].direction, LIGHT[0].directionFactor);
    
    ////dirToLight = -dirToLight;
    
    ////float dotAngle = dot(-dir_to_light,)
	
    //float3 attVec = float3(1.f, dist, dist * dist);
    
    //float diffAtt = LIGHT[0].intensity / dot(attVec, LIGHT[0].attenuation);
    //float specAtt = LIGHT[0].intensity / dot(attVec, LIGHT[0].specularAttunation);
    
    //diffuse = diffuse * LIGHT[0].intensity;
    //diffuse = min(float3(1, 1, 1), diffuse);
    //diffuse = saturate(diffuse);
    //diffuse = diffuse * diffAtt;
	
    //float3 hv = normalize(dir_to_light + dirToEye);
    //float specular = max(0.f, dot(input.world_normal, hv));
    
    //specular = SPECULAR_FACTOR * pow(specular, SPECULAR_POWER);
    //specular *= specAtt;
    
    ////float3 spec = specular * LIGHT[0].color;
    ////spec *= specAtt;
	
    //float3 finalColor = diffuse * surfaceColor + specular;
    
    //return float4(finalColor, surfaceAlpha);
	
    
 
}

//--------------------------------------------------------------------------------------