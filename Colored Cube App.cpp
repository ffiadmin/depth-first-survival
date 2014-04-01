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
	Light mParallelLight;
	//Input* input;
	float score;
	Quad quad1;
	Line line, line2, line3;
	Box mBox, redBox, greenBox, shootBox;

	SoundItem* testSound;

	GameObject testCube;
	FlashLight flashLight;
	Mesh testMesh;
	FlashLightObject flashLightObject;

	Origin origin;

	float spinAmount;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//my addition
	//the location of variable in the shader program like in openGL
	ID3D10EffectVariable* mfxFLIPVar;

	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

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

	if(GetAsyncKeyState('A') & 0x8000) direction.z = -1;
	if(GetAsyncKeyState('D') & 0x8000) direction.z = 1;
	if(GetAsyncKeyState('W') & 0x8000) direction.x = -1;
	if(GetAsyncKeyState('S') & 0x8000) direction.x = 1;
	//if(GetAsyncKeyState('O') & 0x8000) direction.y = 1;
	//if(GetAsyncKeyState('L') & 0x8000) direction.y = -1;

	if(direction!=Vector3(0,0,0))
		D3DXVec3Normalize(&direction,&direction);

	direction *= playerSpeed;
	return direction;
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f), testCube()
{
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
	
	//set up the light
	mParallelLight.dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mParallelLight.ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	buildFX();
	buildVertexLayouts();

	mBox.init(md3dDevice, 1.0f);
	redBox.init(md3dDevice, 1.0f);
	greenBox.init(md3dDevice, 1.0f);
	shootBox.init(md3dDevice, 1.0f);

	flashLight.init(md3dDevice,D3DXCOLOR(1,1,1,1));
	testMesh.init(md3dDevice,1.0f);

	line.init(md3dDevice, 1.0f, DARKBROWN);
	line2.init(md3dDevice, 1.0f, RED);
	line3.init(md3dDevice, 1.0f, GREEN);

	origin.init(&line,&line2,&line3,mfxWVPVar,10);

	testCube.init(&mBox,mfxWVPVar,mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"defaultspec.dds",2,Vector3(20,40,0),Vector3(0,0,0),0,Vector3(5,0.1,5));
	flashLightObject.init(&flashLight,mfxWVPVar,mfxDiffuseMapVar,mfxSpecMapVar,L"butterflyeffect.jpg",L"defaultspec.dds",2,Vector3(0,0,0),Vector3(0,0,0),0,Vector3(1,1,1));

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

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	timer -= dt;
    std::wostringstream outs;   
	/*if(timer<=0)
	{
		outs.clear();
		outs << L"GAME OVER\nFINAL SCORE: " << score;
		mTimer = outs.str();
		return;
	}*/
	testCube.setVelocity(moveCube());
	testCube.update(dt);
	flashLightObject.update(dt);

	outs.precision(2);
    outs << L"Time: " << timer << L"\n";
	outs.precision(3);
	outs << "Score: " << score;
	mTimer = outs.str();

	//if(((int)timer)%2==0)
		//testSound->playSound(testCube.getPosition());

	D3DApp::updateScene(dt);

	// Build the view matrix.
	D3DXVECTOR3 pos(25.0f,50.0f,0.0001f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
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

	// set constants
	//mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mParallelLight, 0, sizeof(Light));
	//sets up stuff for the lighting?
	//mfxWorldVar->SetMatrix((float*)&mCrateWorld);
 
	// Don't transform texture coordinates, so just use identity transformation.
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//draw the origin
	//origin.draw(mView, mProj, mTech);
     
	testCube.draw(mView, mProj, mTech);
	flashLightObject.draw(mView,mProj,mTech);

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

	mTech = mFX->GetTechniqueByName("TexTech");
	
	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 3, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 