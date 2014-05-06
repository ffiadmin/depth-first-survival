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

#include <math.h>
#include "d3dApp.h"
#include "Box.h"
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
#include "playerControls.h"
#include "EnemyHoard.h"
#include "LightObject.h"
#include "Maze.h"

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
	void updateGameState();
	void reloadLevel(int x, int z);

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	void updateL1(float dt);
	void updateL2(float dt);
	void updateL3(float dt);
	void updateL4(float dt);
	void updateSplashScreen(float dt,ID3D10ShaderResourceView* screen);
	void updateLights();
	void drawTitle();
	void drawControls();
	void drawL1();
	void drawL2();
	void drawLose();
	void drawWin();

	void dropBread(Vector3 pos);

	Gamestates gamestate;
	Gamestates nextState;
	Audio* audio;
	playerControls camera;
	Light lights[10];
	int lightsGoingInShader;
	Light* totalLights[100];
	//LightObject lightObject1;
	//Input* input;
	float score;
	Quad quad1;
	Line line, line2, line3;
	Box mBox;
	Maze maze;
	bool perspective;

	int breadNumber;
	int maxBread;

	bool goal;
	int currentKeys, totalKeys;

	D3DXCOLOR ambientLight,hurtLight;

	//SoundItem* testSound;
	Mesh key;
	GameObject keyObject[10];

	GameObject player;
	GameObject endCube;
	Light endLight;
	Light ambientLighting;
	//GameObject floor,wall1,wall2,wall3,wall4;
	//EnemyObject enemy;
	EnemyHoard ghosts;
	//Mesh testMesh;
	FlashLightObject flashLightObject;
	//BatteryObject batteryObject;
	BatteryObject batteries[20];
	LightObject lamps[30];

	GameObject breadCrumbs[30];

	int numLightObjects;
	int numBatteries;

	int l3Ghosts;
	int ghostsKilled;

	Origin origin;
	int oldBLevel;
	float spinAmount;
	int prevLightType;//used for switching light types
	//variable used to keep track of the lights added to the master array so that indexing is kept correctly
	int lightsAdded;
	int mazeX;
	int mazeZ;

	bool perspectiveDebounced;
	bool breadDebounced;

	ID3D10Effect* mFX;
	ID3D10Effect* mFXColor;
	ID3D10EffectTechnique* mTech;
	ID3D10EffectTechnique* mTechColor2;
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
	ID3D10EffectScalarVariable* mfxNumLights;

	//textures and spec maps
	ID3D10ShaderResourceView* brickTexture;
	ID3D10ShaderResourceView* boxTexture;
	ID3D10ShaderResourceView* titleTexture;
	ID3D10ShaderResourceView* controlTexture;
	ID3D10ShaderResourceView* winScreenTexture;
	ID3D10ShaderResourceView* loseScreenTexture;
	ID3D10ShaderResourceView* floorTexture;
	ID3D10ShaderResourceView* ceilingTexture;
	ID3D10ShaderResourceView* l1Splash;
	ID3D10ShaderResourceView* l2Splash;
	ID3D10ShaderResourceView* l3Splash;

	ID3D10ShaderResourceView* standardSpecMap;
	ID3D10ShaderResourceView* brickSpecMap;
	ID3D10ShaderResourceView* iceSpecMap;

	ID3D10ShaderResourceView* splashScreen;

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
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.4f), player(), mRadius(5000), mEyePos(0.0f, 0.0f, 0.0f), ghosts(3,10,50,50)
{
	for(int i = 0; i < 100; i++)
	{
		totalLights[i] = 0;
	}
	breadDebounced = false;
	perspectiveDebounced = false;
	goal = false;
	currentKeys = 0;
	totalKeys = 10;
	numLightObjects = 10;
	numBatteries = 20;
	player.setHealth(10);
	lightsGoingInShader = 10;
	prevLightType = 0;
	score = 0;
	timer = 30;
	lightsAdded = 0;
	l3Ghosts = 10;
	ghostsKilled = 0;
	perspective = false;
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	oldBLevel = 0;
	gamestate = title;
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	//delete audio;
	//delete input;

	brickTexture->Release();
	boxTexture->Release();
	titleTexture->Release();
	controlTexture->Release();
	winScreenTexture->Release();
	loseScreenTexture->Release();
	floorTexture->Release();
	ceilingTexture->Release();

	standardSpecMap->Release();
	brickSpecMap->Release();
	iceSpecMap->Release();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	//delete testSound;
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	//set up textures and spec maps
	//brick texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"brickwork-texture.jpg", 0, 0, &brickTexture, 0 ));
	//box texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"WoodCrate01.dds", 0, 0, &boxTexture, 0 ));
	//title screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"Title Screen.jpg", 0, 0, &titleTexture, 0 ));
	//control screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"Rules Screen.jpg", 0, 0, &controlTexture, 0 ));
	//win screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"You Win.jpg", 0, 0, &winScreenTexture, 0 ));
	//lose screen
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"You Lose.jpg", 0, 0, &loseScreenTexture, 0 ));
	//floor texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"carpet_diffuse.jpg", 0, 0, &floorTexture, 0 ));
	//ceiling texture
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"13.free-brick-textures.jpg", 0, 0, &ceilingTexture, 0 ));
	//level 1 splash
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l1Splash.jpg", 0, 0, &l1Splash, 0 ));
	//level 2 splash
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l2Splash.jpg", 0, 0, &l2Splash, 0 ));
	//level 3 splash
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"l3Splash.jpg", 0, 0, &l3Splash, 0 ));

	//standard spec map
	standardSpecMap;
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"13.free-brick-textures.jpg", 0, 0, &standardSpecMap, 0 ));
	//brick spec map
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"brickwork-bump-map.jpg", 0, 0, &brickSpecMap, 0 ));
	//ice spec map
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"ice.dds", 0, 0, &iceSpecMap, 0 ));

	//0: Parallel
	//1: Point
	//2: Spot

	hurtLight = D3DXCOLOR(1,0,0,1);

	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
	//ambientLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	ambientLighting.ambient = ambientLight;
	ambientLighting.diffuse  = D3DXCOLOR(0.0f, 0.02f, 0.02f, 1.0f);
	ambientLighting.specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	ambientLighting.att.x    = 1.0f;
	ambientLighting.att.y    = 0.0f;
	ambientLighting.att.z    = 0.0f;
	ambientLighting.spotPow  = 20;
	ambientLighting.range    = 100;
	ambientLighting.pos = D3DXVECTOR3(-10,20,-10);
	ambientLighting.dir = D3DXVECTOR3(0, -1, 0);	
	ambientLighting.lightType.x = 0;


	buildFX();
	buildVertexLayouts();

	Dimension d;
	d.x = 10;
	d.z = 10;
	mazeX = d.x;
	mazeZ = d.z;
	maze.init(d,mfxWVPVar,mfxWorldVar,md3dDevice);
	maze.build();
	maze.setTex(brickTexture,brickSpecMap);	
	maze.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	maze.setCeilTex(ceilingTexture,brickSpecMap);
	maze.setFloorTex(floorTexture,brickSpecMap);
	camera.init(&maze);

	mBox.init(md3dDevice, 1.0f);

	//testMesh.init(md3dDevice,1.0f,"surfrev2.dat");

	breadNumber = 0;
	maxBread = 30;

	line.init(md3dDevice, 1.0f, DARKBROWN);
	line2.init(md3dDevice, 1.0f, RED);
	line3.init(md3dDevice, 1.0f, GREEN);

	origin.init(&line,&line2,&line3,mfxWVPVar,10);

	quad1.init(md3dDevice,1,D3DXCOLOR(0.5,0.25,0.1,1.0));

	player.init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(10,0,10),Vector3(0,0,0),0,Vector3(2,2,2));
	player.setTex(boxTexture,standardSpecMap);
	player.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	Location start = maze.getStartPosition();
	start = maze.cellToPx(start);
	player.setPosition(Vector3(start.x+10,0,start.z+10));

	flashLightObject.init(md3dDevice,mfxWVPVar,mfxWorldVar,2,Vector3(0,0,0),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
	flashLightObject.setRotation(Vector3(ToRadian(90),0,0));

	//lightObject1.init(md3dDevice,mfxWVPVar,mfxWorldVar,2,Vector3(10,3,0),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
	//lightObject1.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"ice.dds");
	for(int i = 0; i < numLightObjects; i++)
	{
		lamps[i].init(md3dDevice,mfxWVPVar,mfxWorldVar,2,Vector3(10,3,0),Vector3(0,0,0),0,Vector3(0.75,0.75,0.75));
		lamps[i].setTex(boxTexture,iceSpecMap);
		lamps[i].setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		lamps[i].setPosition(Vector3(spot.x,5,spot.z));
		lamps[i].setColor(D3DXCOLOR(0.2f, 0.5f, 0.3f, 1.0f));
		totalLights[lightsAdded] = lamps[i].getLight();
		lightsAdded++;
	}

	key.init(md3dDevice,1,"item1.txt");
	for(int i = 0; i < totalKeys; i++)
	{
		keyObject[i].init(&key,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		keyObject[i].setPosition(Vector3(spot.x,-1,spot.z));
	}

	endCube.init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(3.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(2,2,2));
	endCube.setTex(boxTexture,iceSpecMap);
	endCube.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	Location end = maze.getEndPosition();
	end = maze.cellToPx(end);
	endCube.setPosition(Vector3(end.x,0,end.z));

	//set up the end light
	endLight.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	endLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 0.3f, 1.0f);
	endLight.specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	endLight.att.x    = 1.0f;
	endLight.att.y    = 0.0f;
	endLight.att.z    = 0.0f;
	endLight.spotPow  = 10;
	endLight.range    = 100;
	endLight.pos = D3DXVECTOR3(end.x,10,end.z);
	endLight.dir = D3DXVECTOR3(0, -1, 0);	
	endLight.lightType.x = 2;

	totalLights[lightsAdded] = &endLight;

	//batteryObject.init(md3dDevice,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(0,0,5),Vector3(0,0,0),0,Vector3(0.25,0.25,0.25));
	for(int i = 0; i < numBatteries; i++)
	{
		batteries[i].init(md3dDevice,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(0,0,5),Vector3(0,0,0),0,Vector3(0.5,0.5,0.5));
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		batteries[i].setPosition(Vector3(spot.x,-1,spot.z));
	}

	ghosts.init(md3dDevice,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(5,0,0),Vector3(0,0,0),10,Vector3(0.25,0.25,0.25));
	ghosts.setTex(boxTexture,iceSpecMap);
	ghosts.setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		totalLights[lightsAdded] = ghosts.getEnemies()[i].getLight();
		lightsAdded++;
	}

	for(int i = 0; i < maxBread; i++)
	{
		breadCrumbs[i].init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(3.0f),Vector3(0,0,0),Vector3(0,0,0),0,Vector3(2,2,2));
		breadCrumbs[i].setInActive();
		breadCrumbs[i].setTexLocVariable(mfxDiffuseMapVar,mfxSpecMapVar);
		breadCrumbs[i].setTex(ceilingTexture,iceSpecMap);
	}

	//Normalize(&mParallelLight.dir,&(flashLightObject.getPosition()-wall1.getPosition()));
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
	camera.setHeightAndWidth(mClientWidth,mClientHeight);

	audio->playCue(MUSIC);

	//input->initialize(this->getMainWnd(), false);  
	//sound object
	/*testSound = new SoundItem(audio,Vector3(0,0,0),50);
	static string sounds[] = {"gun_sound_effect","Light Bulb Breaking-SoundBible.com-53066515"};
	testSound->setSounds(sounds,2);*/
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();
	//float aspect = (float)mClientWidth/mClientHeight;
	//D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
	camera.resize(mProj,0.33f*PI,1.0f,1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	ambientLighting.ambient = ambientLight;
	updateGameState();
	endCube.update(dt);

	if(!breadDebounced && GetAsyncKeyState('B') & 0x8000)
	{
		dropBread(player.getPosition());
		breadDebounced = true;
	}
	if(!(GetAsyncKeyState('B') & 0x8000))
	{
		breadDebounced = false;
	}

	if(!perspectiveDebounced && GetAsyncKeyState('Y') & 0x8000)
	{
		perspective = !perspective;
		perspectiveDebounced = true;
		//ambientLighting.ambient = D3DXCOLOR(1,0.5,0.5,1);
	}
	if(!(GetAsyncKeyState('Y') & 0x8000))
	{
		perspectiveDebounced = false;
		//ambientLighting.ambient = ambientLight;
	}

	for(int i = 0; i < numLightObjects; i++)
	{
		lamps[i].update(dt);
	}

	for(int i = 0; i < numBatteries; i++)
	{
		batteries[i].update(dt);
		if(player.collided(&batteries[i]))
		{
			batteries[i].setInActive();
			flashLightObject.getBattery();
			audio->playCue(BATTERY_CHARGE);
		}
	}

	for(int i = 0; i < maxBread; i++)
	{
		if(breadCrumbs[i].getActiveState())
			breadCrumbs[i].update(dt);
	}

	switch(gamestate)
	{
	case title:
		updateSplashScreen(dt,titleTexture);
		lights[0] = ambientLighting;
		break;
	case controls:
		updateSplashScreen(dt,controlTexture);
		lights[0] = ambientLighting;
		break;
	case level1:
		updateL1(dt);
		updateLights();
		break;
	case level2:
		updateL2(dt);
		updateLights();
		break;
	case level3:
		updateL3(dt);
		updateLights();
		break;
	case level4:
		updateL3(dt);
		updateLights();
		break;
	case gameover:
		updateSplashScreen(dt,loseScreenTexture);
		lights[0] = ambientLighting;
		break;
	case win:
		updateSplashScreen(dt,winScreenTexture);
		lights[0] = ambientLighting;
		break;
	case splash:
		updateSplashScreen(dt,splashScreen);
		lights[0] = ambientLighting;
		break;
	}
}

void ColoredCubeApp::updateLights()
{
	int lightRadius = 50;
	//put in default lights
	int lightIndex = 0;
	if(flashLightObject.getOn())
	{
		lights[lightIndex] = flashLightObject.getLight();
		lightIndex++;
	}

	lights[lightIndex] = ambientLighting;
	lightIndex++;
	Light addingLights[8];
	//calculate the distance of each light from the player
	double distances[100];
	bool added[100];
	for(int i = 0; i < 100; i++)
	{
		added[i] = false;
	}
	int n = 0;
	for(int i = 0; i < lightsAdded; i++)
	{
		Vector3 diff = player.getPosition()-totalLights[i]->pos;
		double distance = sqrtf(abs(Dot(&diff,&diff)));
		distances[i] = distance;
		/*if(distances[i]<lightRadius)
		{
			addingLights[n] = *totalLights[n];
			n++;
		}
		if(n>=8)
		{
			break;
		}*/
	}
	for(int i = lightIndex; i < 10; i++)
	{
		//lights[i] = addingLights[i-2];
		for(int j = 0; j < lightsAdded; j++)
		{
			if(!added[j] && distances[j] < lightRadius)
			{
				added[j] = true;
				lights[i] = *totalLights[j];
				break;
			}
		}
	}
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

	mfxEyePosVar->SetRawValue(&player.getPosition(), 0, sizeof(D3DXVECTOR3));

	//set the number of lights to use
	mfxNumLights->SetInt(lightsGoingInShader);
	//throw lights at shader boooom!!!!!!
	mfxLightVar->SetRawValue(&lights[0], 0, lightsGoingInShader*sizeof(Light));
 
	// Don't transform texture coordinates, so just use identity transformation.
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	mfxTexMtxVar->SetMatrix((float*)&texMtx);

	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//draw the maze
	maze.draw(mTech,mView,mProj);
	
	//draw the keys
	if(gamestate == level1)
	{
		for(int i = 0; i < totalKeys; i++)
		{
			keyObject[i].draw(mView,mProj,mTech);
		}
	}

	//draw the end cube
	if(gamestate == level2)
		endCube.draw(mView,mProj,mTech);

	//draw the origin
	//origin.draw(mView, mProj, mTech);
	
	//draw "crumbs" placed by player
	for(int i = 0; i < maxBread; i++)
	{
		if(breadCrumbs[i].getActiveState())
		{
			breadCrumbs[i].draw(mView,mProj,mTech);
		}
	}

	for(int i = 0; i < numLightObjects; i++)
	{
		lamps[i].draw(mView,mProj,mTech);
	}

	for(int i = 0; i < numBatteries; i++)
	{
		batteries[i].draw(mView,mProj,mTechColor2);
	}

	//flashLightObject.draw(mView,mProj,mTechColor2);
	//flashLightObject.hitBox.draw(mView,mProj,mTechColor2);
	
	player.draw(mView,mProj,mTech);

	if(gamestate == level2 || gamestate == level3)
		ghosts.draw(mView,mProj,mTech);

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::updateL1(float dt)
{
	maze.setCeilTex(ceilingTexture,brickSpecMap);
	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
		
	//detect battery level
	//maybe give gameObjects an audio pointer so they can play cues when necessary...
	if(oldBLevel!=0 && flashLightObject.getPowerLevel()<=0)
	{
		audio->playCue(BATTERY_DIE);
	}
	std::wostringstream outs; 
	oldBLevel = flashLightObject.getPowerLevel();
	auto oldP = player.getPosition();
	//timer -= dt;

	//update the camera
	camera.update(mTheta,mPhi,mRadius,0,dt,player,mView,mEyePos,perspective,true);
	//move the player
	camera.movePlayer(player,30,camera.getTarget(),perspective);
	player.update(dt);
	Location playerLoc;
	playerLoc.x = player.getPosition().x;
	playerLoc.z = player.getPosition().z;
	//collision detection
	if(player.getPosition()!=oldP)
	{
		if(maze.collided(playerLoc))
		{
			player.setPosition(oldP);
			player.setVelocity(Vector3(0,0,0));
			player.update(dt);
		}
	}
	flashLightObject.update(dt);
	
	maze.update(dt);
	
	if(flashLightObject.getPosition()!=(player.getPosition()+(camera.getTarget()*5)))
	{
		flashLightObject.setPosition(player.getPosition() + camera.getTarget()*5);
		int i = 0;
	}

	//orientating the flashlight
	if(flashLightObject.lightSource.dir!=camera.getTarget())
	{
		//vectors for caluclating z-rotation
		Vector2 cameraXY = Vector2(camera.getTarget().x,camera.getTarget().y);
		Vector2 startXY = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.y);
		//vectors for calculating y-rotation
		Vector2 cameraXZ = Vector2(camera.getTarget().x,camera.getTarget().z);
		Vector2 startXZ = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.z);
		//vectors for calculating x-rotation
		Vector2 cameraYZ = Vector2(camera.getTarget().y,camera.getTarget().z);
		Vector2 startYZ = Vector2(flashLightObject.lightSource.dir.y,flashLightObject.lightSource.dir.z);

		float xAngle = flashLightObject.getRotation().x;
		float yAngle = flashLightObject.getRotation().y;
		float zAngle = flashLightObject.getRotation().z;
		float topEquation;
		float bottomEquation;

		topEquation = Dot2(&cameraXY,&startXY);
		bottomEquation = Length2(&cameraXY)*Length2(&startXY);
		if(bottomEquation>0)
		{
			zAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraXZ,&startXZ);
		bottomEquation = Length2(&cameraXZ)*Length2(&startXZ);
		if(bottomEquation>0)
		{
			yAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraYZ,&startYZ);
		bottomEquation = Length2(&cameraYZ)*Length2(&startYZ);
		if(bottomEquation>0)
		{
			xAngle+=acos((topEquation/bottomEquation));
		}

		flashLightObject.setRotation(Vector3(xAngle,yAngle,zAngle));
		flashLightObject.lightSource.dir = camera.getTarget();
	}

	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	outs << "\nKeys Remaining: " << (totalKeys-currentKeys);
	mTimer = outs.str();

	for(int i = 0; i < totalKeys; i++)
	{
		keyObject[i].update(dt);
		if(player.collided(&keyObject[i]))
		{
			currentKeys++;
			keyObject[i].setInActive();
			audio->playCue(ITEM);
		}
	}
}

void ColoredCubeApp::updateL3(float dt)
{
	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);

	//detect battery level
	//maybe give gameObjects an audio pointer so they can play cues when necessary...
	if(oldBLevel!=0 && flashLightObject.getPowerLevel()<=0)
	{
		audio->playCue(BATTERY_DIE);
	}

	oldBLevel = flashLightObject.getPowerLevel();
	auto oldP = player.getPosition();
	timer -= dt;
	std::wostringstream outs; 
	//update the camera
	camera.update(mTheta,mPhi,mRadius,0,dt,player,mView,mEyePos,perspective,true);
	//move the player
	camera.movePlayer(player,30,camera.getTarget(),perspective);
	player.update(dt);
	Location playerLoc;
	playerLoc.x = player.getPosition().x;
	playerLoc.z = player.getPosition().z;
	//collision detection
	if(player.getPosition()!=oldP)
	{
		if(maze.collided(playerLoc))
		{
			player.setPosition(oldP);
			player.setVelocity(Vector3(0,0,0));
			player.update(dt);
		}
	}
	flashLightObject.update(dt);
		
	maze.update(dt);
	
	if(flashLightObject.getPosition()!=(player.getPosition()+(camera.getTarget()*5)))
	{
		flashLightObject.setPosition(player.getPosition() + camera.getTarget()*5);
		int i = 0;
	}

	//orientating the flashlight
	if(flashLightObject.lightSource.dir!=camera.getTarget())
	{
		//vectors for caluclating z-rotation
		Vector2 cameraXY = Vector2(camera.getTarget().x,camera.getTarget().y);
		Vector2 startXY = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.y);
		//vectors for calculating y-rotation
		Vector2 cameraXZ = Vector2(camera.getTarget().x,camera.getTarget().z);
		Vector2 startXZ = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.z);
		//vectors for calculating x-rotation
		Vector2 cameraYZ = Vector2(camera.getTarget().y,camera.getTarget().z);
		Vector2 startYZ = Vector2(flashLightObject.lightSource.dir.y,flashLightObject.lightSource.dir.z);

		float xAngle = flashLightObject.getRotation().x;
		float yAngle = flashLightObject.getRotation().y;
		float zAngle = flashLightObject.getRotation().z;
		float topEquation;
		float bottomEquation;

		topEquation = Dot2(&cameraXY,&startXY);
		bottomEquation = Length2(&cameraXY)*Length2(&startXY);
		if(bottomEquation>0)
		{
			zAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraXZ,&startXZ);
		bottomEquation = Length2(&cameraXZ)*Length2(&startXZ);
		if(bottomEquation>0)
		{
			yAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraYZ,&startYZ);
		bottomEquation = Length2(&cameraYZ)*Length2(&startYZ);
		if(bottomEquation>0)
		{
			xAngle+=acos((topEquation/bottomEquation));
		}

		flashLightObject.setRotation(Vector3(xAngle,yAngle,zAngle));
		flashLightObject.lightSource.dir = camera.getTarget();
	}

	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	mTimer = outs.str();

	ghosts.update(dt,&player,camera.getTarget(),perspective);
	//ghost collision detection
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		//player gets hit by a ghost
		if(player.collided(&ghosts.getEnemies()[i]))
		{
			player.setHealth(player.getHealth()-1);
			lights[1].ambient = hurtLight;
			//make flash take longer
			ghosts.getEnemies()[i].setInActive();
			audio->playCue(P_HIT);
		}
	}
	//flashlight ghost attack
	if(flashLightObject.getOn())
	{
		for(int i = 0; i < ghosts.getNumEnemies(); i++)
		{
			if(flashLightObject.hitTarget(&ghosts.getEnemies()[i]))
			{
				ghosts.getEnemies()[i].decreaseHealth();
				audio->playCue(G_HIT);
			}
		}
	}
	//win scenario
	if(player.collided(&endCube))
	{
		goal = true;
	}
}

void ColoredCubeApp::updateL2(float dt)
{
	std::wostringstream outs;
	outs.precision(2);
	outs << L"Health: " << player.getHealth() << L"\n";
	outs.precision(3);
	outs << "Battery: " << flashLightObject.getPowerLevel();
	outs << "\nBreadcrumbs: " << maxBread-breadNumber;
	outs << "\nGhosts remaining: " << l3Ghosts - ghostsKilled;
	mTimer = outs.str();

	auto oldP = player.getPosition();
	//timer -= dt;

	//update the camera
	camera.update(mTheta,mPhi,mRadius,0,dt,player,mView,mEyePos,perspective,true);
	//move the player
	camera.movePlayer(player,30,camera.getTarget(),perspective);
	player.update(dt);
	Location playerLoc;
	playerLoc.x = player.getPosition().x;
	playerLoc.z = player.getPosition().z;
	//collision detection
	if(player.getPosition()!=oldP)
	{
		if(maze.collided(playerLoc))
		{
			player.setPosition(oldP);
			player.setVelocity(Vector3(0,0,0));
			player.update(dt);
		}
	}

	flashLightObject.update(dt);
	
	if(flashLightObject.getPosition()!=(player.getPosition()+(camera.getTarget()*5)))
	{
		flashLightObject.setPosition(player.getPosition() + camera.getTarget()*5);
		int i = 0;
	}

	//orientating the flashlight
	if(flashLightObject.lightSource.dir!=camera.getTarget())
	{
		//vectors for caluclating z-rotation
		Vector2 cameraXY = Vector2(camera.getTarget().x,camera.getTarget().y);
		Vector2 startXY = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.y);
		//vectors for calculating y-rotation
		Vector2 cameraXZ = Vector2(camera.getTarget().x,camera.getTarget().z);
		Vector2 startXZ = Vector2(flashLightObject.lightSource.dir.x,flashLightObject.lightSource.dir.z);
		//vectors for calculating x-rotation
		Vector2 cameraYZ = Vector2(camera.getTarget().y,camera.getTarget().z);
		Vector2 startYZ = Vector2(flashLightObject.lightSource.dir.y,flashLightObject.lightSource.dir.z);

		float xAngle = flashLightObject.getRotation().x;
		float yAngle = flashLightObject.getRotation().y;
		float zAngle = flashLightObject.getRotation().z;
		float topEquation;
		float bottomEquation;

		topEquation = Dot2(&cameraXY,&startXY);
		bottomEquation = Length2(&cameraXY)*Length2(&startXY);
		if(bottomEquation>0)
		{
			zAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraXZ,&startXZ);
		bottomEquation = Length2(&cameraXZ)*Length2(&startXZ);
		if(bottomEquation>0)
		{
			yAngle+=acos((topEquation/bottomEquation));
		}

		topEquation = Dot2(&cameraYZ,&startYZ);
		bottomEquation = Length2(&cameraYZ)*Length2(&startYZ);
		if(bottomEquation>0)
		{
			xAngle+=acos((topEquation/bottomEquation));
		}

		flashLightObject.setRotation(Vector3(xAngle,yAngle,zAngle));
		flashLightObject.lightSource.dir = camera.getTarget();
	}

	maze.update(dt);

	ghosts.update(dt,&player,camera.getTarget(),perspective);
	//ghost collision detection
	
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		//player gets hit by a ghost
		if(player.collided(&ghosts.getEnemies()[i]))
		{
			player.setHealth(player.getHealth()-1);
			lights[1].ambient = hurtLight;
			//make flash take longer
			ghosts.getEnemies()[i].setInActive();
			audio->playCue(P_HIT);
		}
	}
	//flashlight ghost attack
	if(flashLightObject.getOn())
	{
		for(int i = 0; i < ghosts.getNumEnemies(); i++)
		{
			if(flashLightObject.hitTarget(&ghosts.getEnemies()[i]))
			{
				if(ghosts.getEnemies()[i].decreaseHealth())
					ghostsKilled++;
				audio->playCue(G_HIT);
			}
		}
	}
	//win scenario
	if(ghostsKilled>=l3Ghosts)
	{
		goal = true;
	}
}

void ColoredCubeApp::updateL4(float dt)
{

}

void ColoredCubeApp::updateSplashScreen(float dt,ID3D10ShaderResourceView* screen)
{
	ambientLighting.ambient = D3DXCOLOR(1,1,1,1);
	float rad = 0.0f;
	camera.update(mTheta,mPhi,rad,0,dt,player,mView,mEyePos,true,false);
	//set ceiling texture here
	maze.setCeilTex(screen,brickSpecMap);
	maze.update(dt);
}

void ColoredCubeApp::drawTitle()
{

}

void ColoredCubeApp::drawControls()
{

}

void ColoredCubeApp::drawL1()
{

}

void ColoredCubeApp::drawL2()
{

}

void ColoredCubeApp::drawLose()
{

}

void ColoredCubeApp::drawWin()
{

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
	mTechColor2 = mFX->GetTechniqueByName("ColorTech");
	//mTechStatic = mFX->GetTechniqueByName("StaticTech");

	mfxWVPVar        = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar      = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar     = mFX->GetVariableByName("gEyePosW");
	mfxLightVar      = mFX->GetVariableByName("gLight");
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxNumLights	 = mFX->GetVariableByName("gNumLights")->AsScalar();
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

void ColoredCubeApp::updateGameState()
{
       if(gamestate == title && (GetAsyncKeyState('E') & 0x8000))
       {
			gamestate = controls;
       }
	   if(gamestate == controls && (GetAsyncKeyState('G') & 0x8000))
       {
            gamestate = splash;
			nextState = level1;
			splashScreen = l1Splash;
       }
       if(gamestate == level1 && (currentKeys==totalKeys||(GetAsyncKeyState('P') & 0x8000)))
       {
		    gamestate = splash;
            nextState = level2;
			reloadLevel(10,10);
			splashScreen = l2Splash;
       }
       if(gamestate == level2 && goal ||(GetAsyncKeyState('Q') & 0x8000))
       {
            nextState = level3;
			gamestate = splash;
			reloadLevel(20,20);
			splashScreen = l3Splash;
       }
	   if(gamestate == level3 && goal)
	   {
			gamestate = win;
	   }
	   if(gamestate == level4 && goal)
	   {
		   gamestate = win;//WOOHOO!!!
	   }
	   if(gamestate == splash && (GetAsyncKeyState(VK_SPACE) & 0x8000))
	   {
		   gamestate = nextState;
		   maze.setCeilTex(ceilingTexture,brickSpecMap);
	   }
       if((player.getHealth() <= 0))
       {
              gamestate = gameover;
			  ambientLighting.ambient = ambientLight;
       }
       if((gamestate == gameover ||gamestate==win) && (GetAsyncKeyState('P') & 0x8000))
       {
              gamestate = title;
			  reloadLevel(10,10);
			  player.setHealth(10);
			  goal = false;
       }
}

 void ColoredCubeApp::reloadLevel(int x, int z)
{
	//reset breadcrumbs
	breadNumber = 0;
	for(int i = 0; i < maxBread; i++)
	{
		breadCrumbs[i].setInActive();
	}

	mazeX = x;
	mazeZ = z;
	currentKeys = 0;
	flashLightObject.getBattery();
	//add in the maze reset function
	Dimension d;
	d.x=x;
	d.z = z;
	maze.init(d,mfxWVPVar,mfxWorldVar,md3dDevice);
	maze.build();

	//put the player in the right spot
	Location start = maze.getStartPosition();
	start = maze.cellToPx(start);
	player.setPosition(Vector3(start.x+10,0,start.z+10));

	//put the goal box in the correct location
	Location end = maze.getEndPosition();
	end = maze.cellToPx(end);
	endCube.setPosition(Vector3(end.x,0,end.z));

	//set up the end light
	endLight.ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	endLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 0.3f, 1.0f);
	endLight.specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	endLight.att.x    = 1.0f;
	endLight.att.y    = 0.0f;
	endLight.att.z    = 0.0f;
	endLight.spotPow  = 10;
	endLight.range    = 100;
	endLight.pos = D3DXVECTOR3(end.x,10,end.z);
	endLight.dir = D3DXVECTOR3(0, -1, 0);	
	endLight.lightType.x = 2;

	//reset the maze textures
	maze.setTex(brickTexture,brickSpecMap);	
	maze.setTexLocVariable(mfxDiffuseMapVar, mfxSpecMapVar);
	maze.setCeilTex(ceilingTexture,brickSpecMap);
	maze.setFloorTex(floorTexture,brickSpecMap);

	//place extra items in random cells
	for(int i = 0; i < numLightObjects; i++)
	{
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		lamps[i].setPosition(Vector3(spot.x,5,spot.z));
	}
	for(int i = 0; i < totalKeys; i++)
	{
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		keyObject[i].setPosition(Vector3(spot.x,-1,spot.z));
		keyObject[i].setActive();
	}
	for(int i = 0; i < numBatteries; i++)
	{
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		batteries[i].setPosition(Vector3(spot.x,-1,spot.z));
		batteries[i].setActive();
	}
}

 void ColoredCubeApp::dropBread(Vector3 pos)
 {
	 if(breadNumber>=maxBread)
		 return;
	 breadCrumbs[breadNumber].setPosition(pos);
	 breadCrumbs[breadNumber].setActive();
	 breadNumber++;
 }