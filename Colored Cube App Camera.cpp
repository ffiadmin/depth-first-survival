//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include "QuadObject.h"
#include <d3dx9math.h>
#include "LineObject.h"
#include <sstream>



class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 
	Vector3 moveCube();

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Quad quad;
	Line line;
	Line lineX, lineY, lineZ;
	Box mBox, redBox, blackBox, greenBox;
	GameObject gameObject1, gameObject2, gameObject3, gameObject4, spinner;
	GameObject walls1[wallCount];
	GameObject walls2[wallCount];
	QuadObject quadObject1, quadObject2; 
	LineObject xLine, yLine, zLine;

	float spinAmount;
	float playerSpeed;
	float score;
	float timer;
	std::wstring mTimer;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//my addition
	ID3D10EffectVariable* mfxFLIPVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	D3DXVECTOR3 mEyePos; //Added
	float mRadius; //Added
	float mTheta;
	float mPhi;
	

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

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0), mEyePos(0.0f, 0.0f, 0.0f), 
	mRadius(50.0f), mfxWVPVar(0), mTheta(ToRadian(270.0f)), mPhi(PI*0.5f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	
	mBox.init(md3dDevice, 1.0f, WHITE);
	redBox.init(md3dDevice, 1.0f, RED);
	blackBox.init(md3dDevice, 1.0f, BLACK);
	greenBox.init(md3dDevice, 1.0f, GREEN);
	//line.init(md3dDevice, 10.0f, GREEN);
	lineX.init(md3dDevice, 10.0f, GREEN);
	lineY.init(md3dDevice, 10.0f, RED);
	lineZ.init(md3dDevice, 10.0f, YELLOW);
	quad.init(md3dDevice, 10, CYAN);

	xLine.init(&lineX, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&lineY, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&lineZ, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(90));

	//quad1.setPosition(Vector3(0,-1.2,0));

	spinAmount = 0;
	playerSpeed = 20;
	timer = 300;
	score = 0;
	spinner.init(&redBox, 0, Vector3(0,4,0), Vector3(0,0,0), 0,1);

	gameObject1.init(&greenBox, sqrt(2.0f), Vector3(0,0,0), Vector3(0,0,0), 0,1);
	//gameObject1.setScale(2);
	//gameObject2.init(&redBox, sqrt(2.0f), Vector3(4,0,0), Vector3(0,0,0), 0,1);
	//gameObject3.init(&redBox, sqrt(2.0f), Vector3(-4,0,0), Vector3(0,0,0), 0,1);
	//gameObject4.init(&mBox, sqrt(2.0f), Vector3(0,0,5), Vector3(0,0,0), 0,1);
	//gameObject4.setSpeed(playerSpeed);

	quadObject1.init(&quad, Vector3(0,0,0),1);
	quadObject1.setPosition(Vector3(0,-1.2,0));
	//quadObject1.setRotationX(ToRadian(180));
	//quadObject1.setSpeed(playerSpeed);
	
	quadObject2.init(&quad, Vector3(0,0,0),1);
	quadObject2.setPosition(Vector3(0,-2.2,0));
	//quadObject1.setRotationZ(ToRadian(90));

	for(int i = 0; i < wallCount; ++i)
	{
		walls1[i].init(&redBox, sqrt(2.0f), Vector3(0,0,0), Vector3(0,0,0), 0,1);
		walls1[i].setPosition(Vector3((i%5)*2,2*(i/5),5));
	}
	for(int i = 0; i < wallCount; ++i)
	{
		walls2[i].init(&blackBox, sqrt(2.0f), Vector3(0,0,0), Vector3(0,0,0), 0,1);
		walls2[i].setPosition(Vector3((-i%5)*2,2*(i/5),-5));
	}

	buildFX();
	buildVertexLayouts();

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
	outs.precision(3);
	outs << L"Theta: " << ToDegree(mTheta) << L"\n" 
		<< "Phi: " << ToDegree(mPhi);
	mTimer = outs.str();
	D3DApp::updateScene(dt);
	Vector3 oldPosition = gameObject1.getPosition();
	gameObject1.update(dt);
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);
	//quad1.update(dt);
	quadObject1.update(dt);
	quadObject2.update(dt);

	gameObject1.setVelocity(moveCube());

	for(int i = 0; i < wallCount; ++i)
	{
		walls1[i].update(dt);
	}
	for(int i = 0; i < wallCount; ++i)
	{
		walls2[i].update(dt);
	}

	for(int i = 0; i < wallCount; ++i)
	{
		if (gameObject1.collided(&walls1[i]))
		{
			gameObject1.setPosition(oldPosition);
			walls1[i].setInActive();
		}
	}
	for(int i = 0; i < wallCount; ++i)
	{
		if (gameObject1.collided(&walls2[i]))
		{
			gameObject1.setPosition(oldPosition);
			
		}
	}

	spinAmount += dt ;
	if (ToRadian(spinAmount*40)>2*PI)
		spinAmount = 0;

	if(GetAsyncKeyState('J') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('L') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('I') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('K') & 0x8000)	mPhi += 2.0f*dt;
	if(GetAsyncKeyState('O') & 0x8000)	mRadius -= 25.0f*dt;
	if(GetAsyncKeyState('P') & 0x8000)	mRadius += 25.0f*dt;

	// Restrict the angle mPhi and radius mRadius.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	//Loop mTheta back to zero for maths sake
	if( mTheta >= (PI * 2) ) mTheta = ToRadian(0.1f);
	if( mTheta <= 0.0f ) mTheta = ToRadian(359.99f);

	if( mRadius < 0.0f) mRadius = 0.0f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePos.x =  mRadius*sinf(mPhi)*sinf(mTheta) + gameObject1.getPosition().x;
	mEyePos.z = -mRadius*sinf(mPhi)*cosf(mTheta) + gameObject1.getPosition().z;;
	mEyePos.y =  mRadius*cosf(mPhi) + gameObject1.getPosition().y;

	// Build the view matrix.
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	target.x = gameObject1.getPosition().x;
	target.y = gameObject1.getPosition().y;
	target.z = gameObject1.getPosition().z;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);
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

	// set some variables for the shader
	int foo[1];
	foo[0] = 0;
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//setting the color flip variable in the shader
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));

	//draw the lines
	mWVP = xLine.getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	xLine.setMTech(mTech);
	xLine.draw();
	
	mWVP = yLine.getWorldMatrix() *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	yLine.setMTech(mTech);
	yLine.draw();

	mWVP = zLine.getWorldMatrix() *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	zLine.setMTech(mTech);
	zLine.draw();

	//draw quads
	mWVP = quadObject1.getWorldMatrix()  *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	quadObject1.setMTech(mTech);
	quadObject1.draw();

	mWVP = quadObject2.getWorldMatrix()  *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	quadObject2.setMTech(mTech);
	quadObject2.draw();

	//draw the boxes
	mWVP = gameObject1.getWorldMatrix()  *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	gameObject1.setMTech(mTech);
	gameObject1.draw();

	for(int i = 0; i < wallCount; ++i)
	{
		mWVP = walls1[i].getWorldMatrix()  *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		foo[0] = 0;
		mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
		walls1[i].setMTech(mTech);
		walls1[i].draw();
	}

	for(int i = 0; i < wallCount; ++i)
	{
		mWVP = walls2[i].getWorldMatrix()  *mView*mProj;
		mfxWVPVar->SetMatrix((float*)&mWVP);
		walls2[i].setMTech(mTech);
		walls2[i].draw();
	}

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLACK);
	mSwapChain->Present(0, 0);
}

Vector3 ColoredCubeApp::moveCube()
{
	Vector3 direction = VectorZero;
	if( mTheta <= ToRadian(45.0f) && mTheta >= ToRadian(0.0f))
	{
		if(GetAsyncKeyState('S') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	else if( mTheta <= ToRadian(135.0f) && mTheta > ToRadian(45.0f))
	{
		if(GetAsyncKeyState('A') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('S') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	else if( mTheta > ToRadian(135.0f) && mTheta <= ToRadian(225.0f))
	{
		if(GetAsyncKeyState('W') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('S') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	else if( mTheta <= ToRadian(315.0f) && mTheta > ToRadian(225.0f))
	{
		if(GetAsyncKeyState('D') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('S') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}
	if( mTheta > ToRadian(315.0f) && mTheta <= ToRadian(360.0f))
	{
		if(GetAsyncKeyState('S') & 0x8000)
			direction.z = -1;
		if(GetAsyncKeyState('D') & 0x8000)
			direction.x = 1;
		if(GetAsyncKeyState('W') & 0x8000)
			direction.z = 1;
		if(GetAsyncKeyState('A') & 0x8000)
			direction.x = -1;
		if(GetAsyncKeyState('Z') & 0x8000)
			direction.y = 1;
		if(GetAsyncKeyState('X') & 0x8000)
			direction.y = -1;
	}

	if(direction != VectorZero)
		Normalize(&direction, &direction);

	direction *= playerSpeed;

	return direction;
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
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

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxFLIPVar = mFX->GetVariableByName("flip");

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 