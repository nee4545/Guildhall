#include "ConstantBuffers.hlsl"
// input to the vertex shader - for now, a special input that is the index of the vertex we're drawing

//--------------------------------------------------------------------------------------
// constants
//--------------------------------------------------------------------------------------
// The term 'static' refers to this an built into the shader, and not coming
// from a contsant buffer - which we'll get into later (if you remove static, you'll notice
// this stops working).

static float3 POSITIONS[ 3 ] =
{
    float3( -1.0f , -1.0f , 0.0f ) ,
   float3( 3.0f , -1.0f , 0.0f ) ,
   float3( -1.0f , 3.0f , 0.0f )
};

//--------------------------------------------------------------------------------------

static float2 UVS[ 3 ] =
{
    float2( 0.0f , 1.0f ) ,
   float2( 2.0f , 1.0f ) ,
   float2( 0.0f , -1.0f )
};

//--------------------------------------------------------------------------------------

static float Offsets[ 11 ] =
{
    -5 ,
  -4 ,
  -3 ,
  -2 ,
  -1 ,
  0 ,
  1 ,
  2 ,
  3 ,
  4 ,
  5 ,
};

/*
static float Offsets[ 23 ] =
{
//    -19 ,
//-18 ,
//-17 ,
//-16 ,
//-15 ,
//-14 ,
//-13 ,
//-12 ,
-11 ,
-10 ,
 -9 ,
 -8 ,
 -7 ,
 -6 ,
 -5 ,
 -4 ,
 -3 ,
 -2 ,
 -1 ,
  0 ,
  1 ,
  2 ,
  3 ,
  4 ,
  5 ,
  6 ,
  7 ,
  8 ,
  9 ,
 10 ,
 11 ,
// 12 ,
// 13 ,
// 14 ,
// 15 ,
// 16 ,
// 17 ,
// 18 ,
// 19 ,

};*/

//--------------------------------------------------------------------------------------

/*static float Weights[ 11 ][ 11 ] =
{
    { 0 , 0 , 0 , 0 , 0.000001 , 0.000001 , 0.000001 , 0 , 0 , 0 , 0 } ,
    { 0 , 0 , 0.000001 , 0.000014 , 0.000055 , 0.000088 , 0.000055 , 0.000014 , 0.000001 , 0 , 0 } ,
    { 0 , 0.000001 , 0.000036 , 0.000362 , 0.001445 , 0.002289 , 0.001445 , 0.000362 , 0.000036 , 0.000001 , 0 } ,
    { 0 , 0.000014 , 0.000362 , 0.003672 , 0.014648 , 0.023204 , 0.014648 , 0.003672 , 0.000362 , 0.000014 , 0 } ,
    { 0.000001 , 0.000055 , 0.001445 , 0.014648 , 0.058433 , 0.092564 , 0.058433 , 0.014648 , 0.001445 , 0.000055 , 0.000001 } ,
    { 0.000001 , 0.000088 , 0.002289 , 0.023204 , 0.092564 , 0.146632 , 0.092564 , 0.023204 , 0.002289 , 0.000088 , 0.000001 } ,
    { 0.000001 , 0.000055 , 0.001445 , 0.014648 , 0.058433 , 0.092564 , 0.058433 , 0.014648 , 0.001445 , 0.000055 , 0.000001 } ,
    { 0 , 0.000014 , 0.000362 , 0.003672 , 0.014648 , 0.023204 , 0.014648 , 0.003672 , 0.000362 , 0.000014 , 0 } ,
    { 0 , 0.000001 , 0.000036 , 0.000362 , 0.001445 , 0.002289 , 0.001445 , 0.000362 , 0.000036 , 0.000001 , 0 } ,
    { 0 , 0 , 0.000001 , 0.000014 , 0.000055 , 0.000088 , 0.000055 , 0.000014 , 0.000001 , 0 , 0 } ,
    { 0 , 0 , 0 , 0 , 0.000001 , 0.000001 , 0.000001 , 0 , 0 , 0 , 0 } ,

};*/

static float Weights[ 11 ][ 11 ] =
{
{0.0071 , 0.007427 , 0.007691 , 0.007886 , 0.008005 , 0.008045 , 0.008005 , 0.007886 , 0.007691 , 0.007427 , 0.0071         },
{0.007427 , 0.007768 , 0.008045 , 0.008248 , 0.008373 , 0.008415 , 0.008373 , 0.008248 , 0.008045 , 0.007768 , 0.007427     },
{0.007691 , 0.008045 , 0.008331 , 0.008542 , 0.008671 , 0.008714 , 0.008671 , 0.008542 , 0.008331 , 0.008045 , 0.007691     },
{0.007886 , 0.008248 , 0.008542 , 0.008758 , 0.00889 , 0.008935 , 0.00889 , 0.008758 , 0.008542 , 0.008248 , 0.007886       },
{0.008005 , 0.008373 , 0.008671 , 0.00889 , 0.009025 , 0.00907 , 0.009025 , 0.00889 , 0.008671 , 0.008373 , 0.008005        },
{0.008045 , 0.008415 , 0.008714 , 0.008935 , 0.00907 , 0.009115 , 0.00907 , 0.008935 , 0.008714 , 0.008415 , 0.008045       },
{0.008005 , 0.008373 , 0.008671 , 0.00889 , 0.009025 , 0.00907 , 0.009025 , 0.00889 , 0.008671 , 0.008373 , 0.008005        },
{0.007886 , 0.008248 , 0.008542 , 0.008758 , 0.00889 , 0.008935 , 0.00889 , 0.008758 , 0.008542 , 0.008248 , 0.007886       },
{0.007691 , 0.008045 , 0.008331 , 0.008542 , 0.008671 , 0.008714 , 0.008671 , 0.008542 , 0.008331 , 0.008045 , 0.007691     },
{0.007427 , 0.007768 , 0.008045 , 0.008248 , 0.008373 , 0.008415 , 0.008373 , 0.008248 , 0.008045 , 0.007768 , 0.007427     },
{0.0071 , 0.007427 , 0.007691 , 0.007886 , 0.008005 , 0.008045 , 0.008005 , 0.007886 , 0.007691 , 0.007427 , 0.0071         },

};

/*static float Weights[ 23 ][ 23 ] =
{
{0.000053 , 0.000081 , 0.000118 , 0.000166 , 0.000224 , 0.00029 , 0.000361 , 0.000432 , 0.000496 , 0.000548 , 0.000582 , 0.000594 , 0.000582 , 0.000548 , 0.000496 , 0.000432 , 0.000361 , 0.00029 , 0.000224 , 0.000166 , 0.000118 , 0.000081 , 0.000053       },
{0.000081 , 0.000123 , 0.00018 , 0.000252 , 0.00034 , 0.00044 , 0.000548 , 0.000656 , 0.000754 , 0.000834 , 0.000885 , 0.000903 , 0.000885 , 0.000834 , 0.000754 , 0.000656 , 0.000548 , 0.00044 , 0.00034 , 0.000252 , 0.00018 , 0.000123 , 0.000081           },
{0.000118 , 0.00018 , 0.000262 , 0.000368 , 0.000496 , 0.000643 , 0.000801 , 0.000958 , 0.001102 , 0.001217 , 0.001292 , 0.001318 , 0.001292 , 0.001217 , 0.001102 , 0.000958 , 0.000801 , 0.000643 , 0.000496 , 0.000368 , 0.000262 , 0.00018 , 0.000118       },
{0.000166 , 0.000252 , 0.000368 , 0.000517 , 0.000697 , 0.000903 , 0.001124 , 0.001345 , 0.001546 , 0.001708 , 0.001814 , 0.00185 , 0.001814 , 0.001708 , 0.001546 , 0.001345 , 0.001124 , 0.000903 , 0.000697 , 0.000517 , 0.000368 , 0.000252 , 0.000166      },
{0.000224 , 0.00034 , 0.000496 , 0.000697 , 0.000939 , 0.001217 , 0.001516 , 0.001814 , 0.002085 , 0.002304 , 0.002446 , 0.002495 , 0.002446 , 0.002304 , 0.002085 , 0.001814 , 0.001516 , 0.001217 , 0.000939 , 0.000697 , 0.000496 , 0.00034 , 0.000224       },
{0.00029 , 0.00044 , 0.000643 , 0.000903 , 0.001217 , 0.001577 , 0.001964 , 0.00235 , 0.002702 , 0.002985 , 0.003169 , 0.003233 , 0.003169 , 0.002985 , 0.002702 , 0.00235 , 0.001964 , 0.001577 , 0.001217 , 0.000903 , 0.000643 , 0.00044 , 0.00029           },
{0.000361 , 0.000548 , 0.000801 , 0.001124 , 0.001516 , 0.001964 , 0.002446 , 0.002926 , 0.003365 , 0.003717 , 0.003946 , 0.004026 , 0.003946 , 0.003717 , 0.003365 , 0.002926 , 0.002446 , 0.001964 , 0.001516 , 0.001124 , 0.000801 , 0.000548 , 0.000361     },
{0.000432 , 0.000656 , 0.000958 , 0.001345 , 0.001814 , 0.00235 , 0.002926 , 0.003501 , 0.004026 , 0.004448 , 0.004722 , 0.004817 , 0.004722 , 0.004448 , 0.004026 , 0.003501 , 0.002926 , 0.00235 , 0.001814 , 0.001345 , 0.000958 , 0.000656 , 0.000432       },
{0.000496 , 0.000754 , 0.001102 , 0.001546 , 0.002085 , 0.002702 , 0.003365 , 0.004026 , 0.004628 , 0.005114 , 0.005429 , 0.005538 , 0.005429 , 0.005114 , 0.004628 , 0.004026 , 0.003365 , 0.002702 , 0.002085 , 0.001546 , 0.001102 , 0.000754 , 0.000496     },
{0.000548 , 0.000834 , 0.001217 , 0.001708 , 0.002304 , 0.002985 , 0.003717 , 0.004448 , 0.005114 , 0.005649 , 0.005998 , 0.006118 , 0.005998 , 0.005649 , 0.005114 , 0.004448 , 0.003717 , 0.002985 , 0.002304 , 0.001708 , 0.001217 , 0.000834 , 0.000548     },
{0.000582 , 0.000885 , 0.001292 , 0.001814 , 0.002446 , 0.003169 , 0.003946 , 0.004722 , 0.005429 , 0.005998 , 0.006367 , 0.006495 , 0.006367 , 0.005998 , 0.005429 , 0.004722 , 0.003946 , 0.003169 , 0.002446 , 0.001814 , 0.001292 , 0.000885 , 0.000582     },
{0.000594 , 0.000903 , 0.001318 , 0.00185 , 0.002495 , 0.003233 , 0.004026 , 0.004817 , 0.005538 , 0.006118 , 0.006495 , 0.006626 , 0.006495 , 0.006118 , 0.005538 , 0.004817 , 0.004026 , 0.003233 , 0.002495 , 0.00185 , 0.001318 , 0.000903 , 0.000594       },
{0.000582 , 0.000885 , 0.001292 , 0.001814 , 0.002446 , 0.003169 , 0.003946 , 0.004722 , 0.005429 , 0.005998 , 0.006367 , 0.006495 , 0.006367 , 0.005998 , 0.005429 , 0.004722 , 0.003946 , 0.003169 , 0.002446 , 0.001814 , 0.001292 , 0.000885 , 0.000582     },
{0.000548 , 0.000834 , 0.001217 , 0.001708 , 0.002304 , 0.002985 , 0.003717 , 0.004448 , 0.005114 , 0.005649 , 0.005998 , 0.006118 , 0.005998 , 0.005649 , 0.005114 , 0.004448 , 0.003717 , 0.002985 , 0.002304 , 0.001708 , 0.001217 , 0.000834 , 0.000548     },
{0.000496 , 0.000754 , 0.001102 , 0.001546 , 0.002085 , 0.002702 , 0.003365 , 0.004026 , 0.004628 , 0.005114 , 0.005429 , 0.005538 , 0.005429 , 0.005114 , 0.004628 , 0.004026 , 0.003365 , 0.002702 , 0.002085 , 0.001546 , 0.001102 , 0.000754 , 0.000496     },
{0.000432 , 0.000656 , 0.000958 , 0.001345 , 0.001814 , 0.00235 , 0.002926 , 0.003501 , 0.004026 , 0.004448 , 0.004722 , 0.004817 , 0.004722 , 0.004448 , 0.004026 , 0.003501 , 0.002926 , 0.00235 , 0.001814 , 0.001345 , 0.000958 , 0.000656 , 0.000432       },
{0.000361 , 0.000548 , 0.000801 , 0.001124 , 0.001516 , 0.001964 , 0.002446 , 0.002926 , 0.003365 , 0.003717 , 0.003946 , 0.004026 , 0.003946 , 0.003717 , 0.003365 , 0.002926 , 0.002446 , 0.001964 , 0.001516 , 0.001124 , 0.000801 , 0.000548 , 0.000361     },
{0.00029 , 0.00044 , 0.000643 , 0.000903 , 0.001217 , 0.001577 , 0.001964 , 0.00235 , 0.002702 , 0.002985 , 0.003169 , 0.003233 , 0.003169 , 0.002985 , 0.002702 , 0.00235 , 0.001964 , 0.001577 , 0.001217 , 0.000903 , 0.000643 , 0.00044 , 0.00029           },
{0.000224 , 0.00034 , 0.000496 , 0.000697 , 0.000939 , 0.001217 , 0.001516 , 0.001814 , 0.002085 , 0.002304 , 0.002446 , 0.002495 , 0.002446 , 0.002304 , 0.002085 , 0.001814 , 0.001516 , 0.001217 , 0.000939 , 0.000697 , 0.000496 , 0.00034 , 0.000224       },
{0.000166 , 0.000252 , 0.000368 , 0.000517 , 0.000697 , 0.000903 , 0.001124 , 0.001345 , 0.001546 , 0.001708 , 0.001814 , 0.00185 , 0.001814 , 0.001708 , 0.001546 , 0.001345 , 0.001124 , 0.000903 , 0.000697 , 0.000517 , 0.000368 , 0.000252 , 0.000166      },
{0.000118 , 0.00018 , 0.000262 , 0.000368 , 0.000496 , 0.000643 , 0.000801 , 0.000958 , 0.001102 , 0.001217 , 0.001292 , 0.001318 , 0.001292 , 0.001217 , 0.001102 , 0.000958 , 0.000801 , 0.000643 , 0.000496 , 0.000368 , 0.000262 , 0.00018 , 0.000118       },
{0.000081 , 0.000123 , 0.00018 , 0.000252 , 0.00034 , 0.00044 , 0.000548 , 0.000656 , 0.000754 , 0.000834 , 0.000885 , 0.000903 , 0.000885 , 0.000834 , 0.000754 , 0.000656 , 0.000548 , 0.00044 , 0.00034 , 0.000252 , 0.00018 , 0.000123 , 0.000081           },
{0.000053 , 0.000081 , 0.000118 , 0.000166 , 0.000224 , 0.00029 , 0.000361 , 0.000432 , 0.000496 , 0.000548 , 0.000582 , 0.000594 , 0.000582 , 0.000548 , 0.000496 , 0.000432 , 0.000361 , 0.00029 , 0.000224 , 0.000166 , 0.000118 , 0.000081 , 0.000053       },

    };*/

//--------------------------------------------------------------------------------------

struct vs_input_t
{
    uint vidx : SV_VERTEXID; // SV_* stands for System Variable (ie, built-in by D3D11 and has special meaning)
                                                            // in this case, is the the index of the vertex coming in.
};

//--------------------------------------------------------------------------------------

struct VertexToFragment_t
{
   // SV_POSITION is a semantic - or a name identifying this variable.
   // Usually a semantic can be any name we want - but the SV_* family
   // usually denote special meaning (SV = System Variable I believe)
   // SV_POSITION denotes that this is output in clip space, and will be
   // use for rasterization.  When it is the input (pixel shader stage), it will
   // actually hold the pixel coordinates.

    float4 position : SV_POSITION;
    float2 uv : UV;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VertexToFragment_t VertexFunction( vs_input_t input )
{
    VertexToFragment_t v2f = ( VertexToFragment_t ) 0;

   // The output of a vertex shader is in clip-space, which is a 4D vector
   // so we need to convert out input to a 4D vector.
    
    v2f.position = float4( POSITIONS[ input.vidx ] , 1.0f );
    v2f.uv = UVS[ input.vidx ];

   // And return - this will pass it on to the next stage in the pipeline;
    
    return v2f;
}

Texture2D<float4> blurDiffuse : register( t8 );

float4 FragmentFunction( VertexToFragment_t input ) : SV_Target0
{

//--------------------------------------------------------------------------------------
//              SAMPLE THE TEXTURES
//--------------------------------------------------------------------------------------    
  
    float2 textureDimensions;
    blurDiffuse.GetDimensions( textureDimensions.x , textureDimensions.y );
    
    float PixelWidth = 1.0f / textureDimensions.x;
    float PixelHeight = 1.0f / textureDimensions.y;

    float4 Color = { 0 , 0 , 0 , 0 };

    float2 Blur;
        
    for( int x = 0 ; x < 11 ; x++ )
    {
        Blur.x = input.uv.x + Offsets[ x ] * PixelWidth;
        for( int y = 0 ; y < 11 ; y++ )
        {
            Blur.y = input.uv.y + Offsets[ y ] * PixelHeight;
            Color += blurDiffuse.Sample( sSampler , Blur ) * Weights[ x ][ y ];
        }
    }
     
    //return float4( Blur.x , Blur.y , 0 , 1 );
    return Color;
}

//--------------------------------------------------------------------------------------