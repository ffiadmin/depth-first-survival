//=============================================================================
// tex.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms, lights, and textures geometry.
//=============================================================================


#include "lighthelper.fx"
 
 
cbuffer cbPerFrame
{
	Light gLight[100];
	float3 gEyePosW;
	int gNumLights;
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

	diffuseTexel;
	specTexel;
	//diffuse+=pIn.diffuse;
	//spec+=pIn.spec;

	// Map [0,1] --> [0,256]
	specTexel.a *= 256.0f;
	
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(pIn.normalW);
    
	// Compute the lit color for this pixel.
    
	float3 litColor = float3(0,0,0);
	

	for(uint i = 0; i < gNumLights.x; i++)
	{
		SurfaceInfo v = {pIn.posW, normalW, diffuseTexel, specTexel};
		if(gLight[i].lightType.x == 0)
		{
			litColor += ParallelLight(v, gLight[i], gEyePosW);
		}
		else if(gLight[i].lightType.x == 1)
		{
			litColor += PointLight(v, gLight[i], gEyePosW);
		}
		else if(gLight[i].lightType.x == 2)
		{
			litColor += Spotlight(v, gLight[i], gEyePosW);
		}
	}

    return float4(litColor, pIn.diffuse.a);
}

float4 PSColor(VS_OUT pIn) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);
   
   
    SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse/10, pIn.spec/10};
    
    float3 litColor = float3(0,0,0);
	
	for(uint i = 0; i < gNumLights; i++)
	{
		if(gLight[i].lightType.x == 0)
		{
			litColor += ParallelLight(v, gLight[i], gEyePosW);
		}
		else if(gLight[i].lightType.x == 1)
		{
			litColor += PointLight(v, gLight[i], gEyePosW);
		}
		else if(gLight[i].lightType.x == 2)
		{
			litColor += Spotlight(v, gLight[i], gEyePosW);
		}
	}
	
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

technique10 ColorTech
{
	pass P1
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSColor() ) );
	}
}
