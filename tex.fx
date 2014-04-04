//=============================================================================
// tex.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms, lights, and textures geometry.
//=============================================================================


#include "lighthelper.fx"
 
 
cbuffer cbPerFrame
{
	Light gLight;
	float3 gEyePosW;
};

bool gSpecularEnabled;

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP; 
	float4x4 gTexMtx;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gSpecMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float4 color   : COLOR;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
		
	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.texC  = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx);
	vOut.diffuse = vIn.diffuse;
	vOut.spec = vIn.spec;
	
	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target
{
	// Get materials from texture maps.
	float4 diffuseTexel = gDiffuseMap.Sample( gTriLinearSam, pIn.texC );
	float4 specTexel    = gSpecMap.Sample( gTriLinearSam, pIn.texC );

	diffuseTexel/=10;
	specTexel/=10;
	//diffuse+=pIn.diffuse;
	//spec+=pIn.spec;

	// Map [0,1] --> [0,256]
	specTexel.a *= 256.0f;
	
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(pIn.normalW);
    
	// Compute the lit color for this pixel.
    SurfaceInfo v = {pIn.posW, normalW, diffuseTexel, specTexel};// pIn.diffuse, pIn.spec};
	float3 litColor;
	//litColor = ParallelLight(v, gLight, gEyePosW);
	if(gLight.lightType == 0)
	{
		litColor = ParallelLight(v, gLight, gEyePosW);
	}
	else if(gLight.lightType == 1)
	{
		litColor = PointLight(v, gLight, gEyePosW);
	}
	else if(gLight.lightType == 2)
	{
		litColor = Spotlight(v, gLight, gEyePosW);
	}
    //litColor+=diffuseTexel;
	//litColor+=specTexel;
    return float4(litColor, pIn.diffuse.a);
}

technique10 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
