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
	float4x4 gViewProj;
	float alphaValue;
};

bool gSpecularEnabled;

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWVP; 
	float4x4 gTexMtx;
};

cbuffer cbFixed
{
	float gFogStart = 5.0f;
	float gFogRange = 140.0f;
	float3 gFogColor = {0.7f,0.7f,0.7f};
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gSpecMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float4 color   : COLOR;
	float2 sizeW   : SIZE;
};

struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
	float2 sizeW   : SIZE;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
	float fogLerp  : FOG;
};

struct GS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 texC    : TEXCOORD;
    uint primID    : SV_PrimitiveID;
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
	
	float d = distance(vOut.posW,gEyePosW);
	vOut.fogLerp = saturate( (d-gFogStart)/gFogRange );

	return vOut;
}

VS_OUT VSFogCeil(VS_IN vIn)
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
	
	float d = distance(vOut.posW,gEyePosW);
	d = d-distance(vOut.posW,vIn.posL);
	vOut.fogLerp = saturate( (d-gFogStart)/gFogRange );

	return vOut;
}

VS_OUT VSBillboard(VS_IN vIn)
{
	VS_OUT vOut;
	
	// Just pass data into geometry shader stage.
	vOut.posW  = mul(float4(vIn.posL,1),gWorld);
	vOut.posH  = mul(float4(vIn.posL,1.0f), gWVP);
	vOut.sizeW    = vIn.sizeW;

	return vOut;
}

[maxvertexcount(4)]
void GS(point VS_OUT gIn[1], 
        uint primID : SV_PrimitiveID, 
        inout TriangleStream<GS_OUT> triStream)
{	
	//
	// Compute 4 triangle strip vertices (quad) in local space.
	// The quad faces down the +z axis in local space.
	//
	float halfWidth  = 0.5f*gIn[0].sizeW.x;
	float halfHeight = 0.5f*gIn[0].sizeW.y;
	
	float4 v[4];
	v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
	v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
	v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
	v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
	//
	// Compute texture coordinates to stretch texture over quad.
	//
	float2 texC[4];
	texC[0] = float2(0.0f, 1.0f);
	texC[1] = float2(1.0f, 1.0f);
	texC[2] = float2(0.0f, 0.0f);
	texC[3] = float2(1.0f, 0.0f);
	//
	// Compute world matrix so that billboard is aligned with
	// the y-axis and faces the camera.
	//
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosW - gIn[0].posW;
	look.y = 0.0f; // y-axis aligned, so project to xz-plane
	look = normalize(look);
	float3 right = cross(up, look);
	
	float4x4 W;
	W[0] = float4(right,          0.0f);
	W[1] = float4(up,             0.0f);
	W[2] = float4(look,           0.0f);
	W[3] = float4(gIn[0].posW, 1.0f);

	float4x4 WVP = mul(W,gViewProj);
	
	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	GS_OUT gOut;
	[unroll]
	for(int i = 0; i < 4; ++i)
	{
		gOut.posH     = mul(v[i], WVP);
		gOut.posW     = mul(v[i], W);
		gOut.normalW  = look;
		gOut.texC     = texC[i];
		gOut.primID   = primID;
		
		triStream.Append(gOut);
	}
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
	

	for(uint i = 0; i < gNumLights; i++)
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

    return float4(litColor, alphaValue);
}

float4 PSFog(VS_OUT pIn) : SV_Target
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
	

	for(uint i = 0; i < gNumLights; i++)
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

	float3 foggedColor = lerp(litColor,gFogColor, pIn.fogLerp);

    return float4(foggedColor, alphaValue);
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

    return float4(litColor, alphaValue);
}

float4 PSEmmisive(VS_OUT pIn) : SV_Target
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

	litColor += float3(0.05,0.05,0);

    return float4(litColor, alphaValue);
}

float4 PSFogColor(VS_OUT pIn) : SV_Target
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
	
	float3 foggedColor = lerp(litColor,gFogColor, pIn.fogLerp);

    return float4(foggedColor, alphaValue);
}

float4 PSBillboard(GS_OUT pIn) : SV_Target
{
	// Get materials from texture maps.
	float3 uvw = float3(pIn.texC, pIn.primID%4);
	float4 diffuse = gDiffuseMap.Sample( gTriLinearSam, uvw );
 
	// Discard pixel if texture alpha < 0.25.  Note that we do this
	// test as soon as possible so that we can potentially exit the shader 
	// early, thereby skipping the rest of the shader code.
	clip(diffuse.a - 0.25f);
	
	// Don't light tree billboards, just use texture color.
    //return float4(1,1,1,1);
    return diffuse;
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

technique10 BillboardTech
{
	pass P2
	{
		SetVertexShader( CompileShader(   vs_4_0, VSBillboard() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader(    ps_4_0, PSBillboard() ) );
	}
}

technique10 fogTech
{
	pass P3
	{
		SetVertexShader( CompileShader(   vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(    ps_4_0, PSFog() ) );		
	}
}

technique10 fogColorTech
{
	pass P3
	{
		SetVertexShader( CompileShader(   vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(    ps_4_0, PSFogColor() ) );		
	}
}

//special equations meant to make the floor and ceiling of the maze look decent in the fog based level
technique10 ceilingFogTech
{
	pass P3
	{
		SetVertexShader( CompileShader(   vs_4_0, VSFogCeil() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(    ps_4_0, PSFog() ) );		
	}
}

technique10 emmisiveTech
{
	pass P4
	{
		SetVertexShader( CompileShader(   vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader(    ps_4_0, PSEmmisive() ) );		
	}
}