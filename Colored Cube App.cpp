//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

//update postion
//update world matrix of where to draw
//multiple world matrix tell it to draw

#include "d3dApp.h"
#include "Box.h"
#include "flashLight.h"
#include "GameObject.h"
#include "flashLightObject.h"
#include "BatteryObject.h"
#include "Line.h"
#include "Quad.h"
#include "Origin.h"
#include "audio.h"
#include <d3dx9math.h>
#include "gameError.h"
#include "input.h"
#include <sstream>
#include "LineObject.h"
#include "SoundItem.h"
#include "Mesh.h"
#include "FPCamera.h"
#include "FPMovement.h"
#include "TPMovement.h"
#include "TPCamera.h"
#include "EnemyObject.h"

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	Vector3 moveCube();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Audio* audio;
	TPCamera playerCamera;
	TPMovement playerCameraMovement;
	Light mParallelLight;
	//Input* input;
	float score;
	Quad quad1;
	Line line, line2, line3;
	Box mBox;

	SoundItem* testSound;

	GameObject testCube;
	GameObject floor,wall1,wall2,wall3,wall4;
	FlashLight flashLight;
	EnemyObject enemy;
	Mesh testMesh;
	FlashLightObject flashLightObject;
	BatteryObject batteryObject;

	Origin origin;

	float spinAmount;
	int prevLightType;//used for switching light types

	ID3D10Effect* mFX;
	ID3D10Effect* mFXColor;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectTechnique* mTechColor;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWVPVarColor;
	//my addition
	//the location of variable in the shader program like in openGL
	ID3D10EffectVariable* mfxFLIPVar;

	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectMatrixVariable* mfxWorldVarColor;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxEyePosVarColor;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectVariable* mfxLightVarColor;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	D3DXVECTOR3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;

    float timer;
    std::wstring mTimer;
	HRESULT hr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

Vector3 ColoredCubeApp::moveCube()
{
	Vector3 direction = Vector3(0,0,0);
	int playerSpeed = 20;

	if(GetAsyncKeyState('J') & 0x8000) direction.z = -1;
	if(GetAsyncKeyState('L') & 0x8000) direction.z = 1;
	if(GetAsyncKeyState('I') & 0x8000) direction.x = -1;
	if(GetAsyncKeyState('K') & 0x8000) direction.x = 1;
	//if(GetAsyncKeyState('O') & 0x8000) direction.y = 1;
	//if(GetAsyncKeyState('L') & 0x8000) direction.y = -1;

	if(direction!=Vector3(0,0,0))
		D3DXVec3Normalize(&direction,&direction);

	direction *= playerSpeed;
	return direction;
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.4f), testCube(), mRadius(5), mEyePos(0.0f, 0.0f, 0.0f)
{
	prevLightType = 0;
	mTechColor = 0;
	mFXColor = 0;
	mfxWVPVarColor = 0;
	score = 0;
	timer = 30;
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	delete audio;
	//delete input;

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	delete testSound;
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	//0: Parallel
	//1: Point
	//2: Spot

	mParallelLight.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.att.x    = 1.0f;
	mParallelLight.att.y    = 0.0f;
	mParallelLight.att.z    = 0.0f;
	mParallelLight.spotPow  = 1;
	mParallelLight.range    = 50;
	mParallelLight.pos = D3DXVECTOR3(0,5,0);
	mParallelLight.dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	//mParallelLight.att = Vector3(10,10,10);
	
	mParallelLight.lightType = 1;
	prevLightType = mParallelLight.lightType;

	buildFX();
	buildVertexLayouts();

	mBox.init(md3dDevice, 1.0f);

	flashLight.init(md3dDevice,D3DXCOLOR(1,1,1,1));
	testMesh.init(md3dDevice,1.0f,"surfrev2.dat");

	line.init(md3dDevice, 1.0f, DARKBROWN);
	line2.init(md3dDevice, 1.0f, RED);
	line3.init(md3dDevice, 1.0f, GREEN);

	origin.init(&line,&line2,&line3,mfxWVPVar,10);

	//make the texture init in another function so it is not an issue switching between shaders
	floor.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,-3,0),Vector3(0,0,0),0,Vector3(20,0.1,20));
	floor.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"ice.dds");

	wall1.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(-20,-3+20,0),Vector3(0,0,0),0,Vector3(0.1,20,20));
	wall2.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(20,-3+20,0),Vector3(0,0,0),0,Vector3(0.1,20,20));
	wall3.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,-3+20,20),Vector3(0,0,0),0,Vector3(20,20,0.1));
	wall4.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,-3+20,-20),Vector3(0,0,0),0,Vector3(20,20,0.1));
	floor.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"defaultspec.dds");

	testCube.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,0,0),Vector3(0,0,0),0,Vector3(1,1,1));
	testCube.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"defaultspec.dds");

	flashLightObject.init(md3dDevice,mfxWVPVarColor,mfxWorldVarColor,2,Vector3(0,0,0),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
	flashLightObject.setRotation(Vector3(0,0,ToRadian(90)));

	batteryObject.init(md3dDevice,mfxWVPVarColor,mfxWorldVarColor,2,Vector3(0,0,5),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
	enemy.init(md3dDevice,mfxWVPVarColor,mfxWorldVarColor,2,Vector3(5,0,0),Vector3(0,0,0),10,Vector3(0.25,0.25,0.25));
	
	//Normalize(&mParallelLight.dir,&(flashLightObject.getPosition()-wall1.getPosition()));

	quad1.init(md3dDevice, 10, CYAN);
	quad1.setPosition(Vector3(0,0,0));

	spinAmount = 0;

	// init sound system
    audio = new Audio();
    if (*WAVE_BANK != '\0' && *SOUND_BANK != '\0')  // if sound files defined
    {
        if( FAILED( hr = audio->initialize() ) )
        {
			exit(1);
            //if( hr == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
            //    throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system because media file not found."));
            //else
            //    throw(GameError(gameErrorNS::FATAL_ERROR, "Failed to initialize sound system."));
        }
    }
	//set up the camera
	playerCamera.setHeightAndWidth(mClientWidth,mClientHeight);

	//audio->playCue(MUSIC);

	//input->initialize(this->getMainWnd(), false);  
	//sound object
	testSound = new SoundItem(audio,Vector3(0,0,0),50);
	static string sounds[] = {"gun_sound_effect","Light Bulb Breaking-SoundBible.com-53066515"};
	testSound->setSounds(sounds,2);
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();
	//float aspect = (float)mClientWidth/mClientHeight;
	//D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
	playerCamera.resize(mProj,0.25f*PI,1.0f,1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	timer -= dt;
    std::wostringstream outs; 
	//update the camera
	playerCamera.update(mTheta,mPhi,mRadius,0,dt,testCube,mView,mEyePos);
	//move the player
	playerCameraMovement.movePlayer(testCube,10,mTheta);
	//mParallelLight.pos = mEyePos;
	//testCube.setVelocity(moveCube());
	//flashLightObject.setVelocity(moveCube());
	//batteryObject.setVelocity(moveCube());
	testCube.update(dt);
	flashLightObject.update(dt);
	batteryObject.update(dt);
	enemy.update(dt,&testCube);
	quad1.update(dt);
	floor.update(dt);
	wall1.update(dt);
	wall2.update(dt);
	wall3.update(dt);
	wall4.update(dt);

	//mParallelLight.pos = testCube.getPosition();
	//set up the flashlight light direction based on the direction the geometry is pointing
	//D3DXVec3Normalize(&mParallelLight.dir, &(playerCamera.getTarget()-testCube.getPosition()));

	outs.precision(2);
    outs << L"Time: " << timer << L"\n";
	outs.precision(3);
	outs << "Score: " << score;
	mTimer = outs.str();
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	mfxEyePosVar->SetRawValue(&testCube.getPosition(), 0, sizeof(D3DXVECTOR3));
	mfxEyePosVarColor->SetRawValue(&testCube.getPosition(), 0, sizeof(D3DXVECTOR3));

	// set constants
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	if(testCube.collided(&enemy))
	{
		if(mParallelLight.lightType<3)
			prevLightType = mParallelLight.lightType;
		mParallelLight.lightType = 3;
	}
	else
	{
		mParallelLight.lightType = prevLightType;
	}
	
	//mParallelLight.lightType = 2;
	mfxLightVarColor->SetRawValue(&mParallelLight, 0, sizeof(Light));
 
	// Don't transform texture coordinates, so just use identity transformation.
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//draw the origin
	origin.draw(mView, mProj, mTech);
     
	testCube.draw(mView, mProj, mTech);
	
	flashLightObject.draw(mView,mProj,mTechColor);
	
	batteryObject.draw(mView,mProj,mTechColor);
	
	enemy.draw(mView,mProj,mTechColor);

	floor.draw(mView, mProj, mTech);
	wall1.draw(mView, mProj, mTech);
	wall2.draw(mView, mProj, mTech);
	wall3.draw(mView, mProj, mTech);
	wall4.draw(mView, mProj, mTech);

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    //shaderFlags |= D3D10_SHADER_DEBUG;
	//shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
  
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"tex.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	hr = D3DX10CreateEffectFromFile(L"lighting.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFXColor, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("TexTech");
	mTechColor = mFXColor->GetTechniqueByName("LightTech");

	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();

	mfxWVPVarColor	 = mFXColor->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVarColor = mFXColor->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVarColor= mFXColor->GetVariableByName("gEyePosW");
	mfxLightVarColor = mFXColor->GetVariableByName("gLight");
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 6, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 