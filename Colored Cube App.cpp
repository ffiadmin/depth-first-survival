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
	void updateTitle(float dt);
	void updateControls(float dt);
	void updateL1(float dt);
	void updateL2(float dt);
	void updateLose(float dt);
	void updateWin(float dt);
	void drawTitle();
	void drawControls();
	void drawL1();
	void drawL2();
	void drawLose();
	void drawWin();

	Gamestates gamestate;
	Audio* audio;
	playerControls camera;
	Light lights[15];
	//LightObject lightObject1;
	//Input* input;
	float score;
	Quad quad1;
	Line line, line2, line3;
	Box mBox;
	Maze maze;
	bool perspective;

	bool goal;
	int currentKeys, totalKeys;

	D3DXCOLOR ambientLight,hurtLight;

	//SoundItem* testSound;
	Mesh key;
	GameObject keyObject[10];

	GameObject player;
	GameObject endCube;
	Light endLight;
	//GameObject floor,wall1,wall2,wall3,wall4;
	//EnemyObject enemy;
	EnemyHoard ghosts;
	//Mesh testMesh;
	FlashLightObject flashLightObject;
	//BatteryObject batteryObject;
	BatteryObject batteries[20];
	LightObject lamps[10];

	int numLightObjects;
	int numBatteries;

	Origin origin;
	int oldBLevel;
	float spinAmount;
	int prevLightType;//used for switching light types
	int numLights;
	int mazeX;
	int mazeZ;

	bool once;
	bool onceAgain;
	bool onceAgainStart;
	bool onceAgainEnd;

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
	once = true;
	onceAgain = true;
	onceAgainStart = true;
	onceAgainEnd = true;
	goal = false;
	currentKeys = 0;
	totalKeys = 10;
	numLightObjects = 5;
	numBatteries = 20;
	player.setHealth(10);
	numLights=10;
	prevLightType = 0;
	score = 0;
	timer = 30;
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

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	//delete testSound;
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	//0: Parallel
	//1: Point
	//2: Spot

	hurtLight = D3DXCOLOR(1,0,0,1);

	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
	//ambientLight = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	lights[1].ambient  = ambientLight;
	lights[1].diffuse  = D3DXCOLOR(0.0f, 0.02f, 0.02f, 1.0f);
	lights[1].specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1].att.x    = 1.0f;
	lights[1].att.y    = 0.0f;
	lights[1].att.z    = 0.0f;
	lights[1].spotPow  = 20;
	lights[1].range    = 100;
	lights[1].pos = D3DXVECTOR3(-10,20,-10);
	lights[1].dir = D3DXVECTOR3(0, -1, 0);	
	lights[1].lightType.x = 0;


	buildFX();
	buildVertexLayouts();

	Dimension d;
	d.x = 10;
	d.z = 10;
	mazeX = d.x;
	mazeZ = d.z;
	maze.init(d,mfxWVPVar,mfxWorldVar,md3dDevice);
	maze.build();
	maze.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"brickwork-texture.jpg",L"brickwork-bump-map.jpg");	
	maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"13.free-brick-textures.jpg",L"brickwork-bump-map.jpg");
	maze.setFloorTex(mfxDiffuseMapVar,mfxSpecMapVar,L"carpet_diffuse.jpg",L"brickwork-bump-map.jpg");
	camera.init(&maze);

	mBox.init(md3dDevice, 1.0f);

	//testMesh.init(md3dDevice,1.0f,"surfrev2.dat");

	line.init(md3dDevice, 1.0f, DARKBROWN);
	line2.init(md3dDevice, 1.0f, RED);
	line3.init(md3dDevice, 1.0f, GREEN);

	origin.init(&line,&line2,&line3,mfxWVPVar,10);

	quad1.init(md3dDevice,1,D3DXCOLOR(0.5,0.25,0.1,1.0));

	//make the texture init in another function so it is not an issue switching between shaders
	/*floor.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,-3,0),Vector3(0,0,0),0,Vector3(20,0.1,20));
	floor.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"defaultspec.dds");

	wall1.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(-20,-3+20,0),Vector3(0,0,0),0,Vector3(0.1,20,20));
	wall2.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(20,-3+20,0),Vector3(0,0,0),0,Vector3(0.1,20,20));
	wall3.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,-3+20,20),Vector3(0,0,0),0,Vector3(20,20,0.1));
	wall4.init(&mBox,mfxWVPVar,mfxWorldVar,2,Vector3(0,-3+20,-20),Vector3(0,0,0),0,Vector3(20,20,0.1));*/

	player.init(&mBox,mfxWVPVar,mfxWorldVar,sqrt(2.0f),Vector3(10,0,10),Vector3(0,0,0),0,Vector3(1,1,1));
	player.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"brickwork-texture.jpg",L"brickwork-bump-map.jpg");
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
		lamps[i].setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"ice.dds");
		Location l;
		l.x = rand()%mazeX;
		l.z = rand()%mazeZ;
		auto spot = maze.cellToPx(l);
		lamps[i].setPosition(Vector3(spot.x,5,spot.z));
		lamps[i].setColor(D3DXCOLOR(0.2f, 0.5f, 0.3f, 1.0f));
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
	endCube.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"ice.dds");
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
	ghosts.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"WoodCrate01.dds",L"ice.dds");
	
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
	updateGameState();
	if(player.getHealth()<=0)
	{
		gamestate = gameover;
		maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"You Lose.jpg",L"brickwork-bump-map.jpg");
		maze.update(dt);
	}

	endCube.update(dt);

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
	switch(gamestate)
	{
	case title:
		updateTitle(dt);
		break;
	case controls:
		updateControls(dt);
		break;
	case level1:
		updateL1(dt);
		break;
	case level2:
		updateL2(dt);
		break;
	case gameover:
		updateLose(dt);
		break;
	case win:
		updateWin(dt);
		break;
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
	mfxNumLights->SetInt(numLights);

	// set the light array
	lights[0] = flashLightObject.lightSource;
	//lights[2] = lightObject1.getLight();
	if(gamestate == level2)
	{
		for(int i = 0; i < ghosts.getNumEnemies(); i++)
		{
			lights[2+i] = ghosts.getEnemies()[i].getLight();
		}
	}
	for(int i = 0; i < numLightObjects; i++)
	{
		lights[2+ghosts.getNumEnemies()+i] = lamps[i].getLight();
	}
	lights[numLights-1] = endLight;

	mfxLightVar->SetRawValue(&lights[0], 0, numLights*sizeof(Light));
 
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
	endCube.draw(mView,mProj,mTech);

	//draw the origin
	origin.draw(mView, mProj, mTech);
	
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
	
	//batteryObject.draw(mView,mProj,mTechColor2);
	//player.draw(mView,mProj,mTechColor2);
	ghosts.draw(mView,mProj,mTech);

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mTimer.c_str(), -1, &R, DT_NOCLIP, BLUE);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::updateTitle(float dt)
{
	float rad = 0.0f;
	camera.update(mTheta,mPhi,rad,0,dt,player,mView,mEyePos,true);
	maze.update(dt);
	if(once)
	{
		maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"Title Screen.jpg",L"brickwork-bump-map.jpg");
		once = false;
	}
	ambientLight = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	//set ceiling texture here
}

void ColoredCubeApp::updateControls(float dt)
{
	float rad = 0.0f;
	camera.update(mTheta,mPhi,rad,0,dt,player,mView,mEyePos,true);
	maze.update(dt);
	if(onceAgain)
	{
		maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"Rules Screen.jpg",L"brickwork-bump-map.jpg");
		onceAgain = false;
	}
	ambientLight = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	//set ceiling texture here
}

void ColoredCubeApp::updateL1(float dt)
{
	if(onceAgainStart)
	{
		maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"13.free-brick-textures.jpg",L"brickwork-bump-map.jpg");
		onceAgainStart = false;
	}
	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
		
	if(GetAsyncKeyState('Y') & 0x8000)
		perspective = true;
	else
		perspective = false;
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
	camera.update(mTheta,mPhi,mRadius,0,dt,player,mView,mEyePos,perspective);
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

void ColoredCubeApp::updateL2(float dt)
{
	ambientLight = D3DXCOLOR(0.3f, 0.03f, 0.2f, 1.0f);
		
	if(GetAsyncKeyState('Y') & 0x8000)
		perspective = true;
	else
		perspective = false;
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
	camera.update(mTheta,mPhi,mRadius,0,dt,player,mView,mEyePos,perspective);
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
	mTimer = outs.str();

	ghosts.update(dt,&player);
	//ghost collision detection
	lights[1].ambient = ambientLight;
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
	for(int i = 0; i < ghosts.getNumEnemies(); i++)
	{
		if(flashLightObject.hitTarget(&ghosts.getEnemies()[i]))
		{
			ghosts.getEnemies()[i].decreaseHealth();
			audio->playCue(G_HIT);
		}
	}
	//win scenario
	if(player.collided(&endCube))
	{
		goal = true;
	}
}

void ColoredCubeApp::updateLose(float dt)
{
	float rad = 0.0f;
	camera.update(mTheta,mPhi,rad,0,dt,player,mView,mEyePos,true);
	//set ceiling texture here
	if(onceAgainEnd)
	{
		maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"You Lose.jpg",L"brickwork-bump-map.jpg");
		onceAgainEnd = false;
		onceAgainStart = true;
	}
}

void ColoredCubeApp::updateWin(float dt)
{
	float rad = 0.0f;
	camera.update(mTheta,mPhi,rad,0,dt,player,mView,mEyePos,true);
	//set ceiling texture here
	if(onceAgainEnd)
	{
		maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"You Win.jpg",L"brickwork-bump-map.jpg");
		onceAgainEnd = false;
		onceAgainStart = true;
	}
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
              gamestate = level1;
       }
       if(gamestate == level1 && (currentKeys==totalKeys||(GetAsyncKeyState('P') & 0x8000)))
       {
              gamestate = level2;
			  reloadLevel(20,20);
       }
       if(gamestate == level2 && goal)
       {
              gamestate = win;
       }
       if((gamestate == level1 || gamestate == level2) && player.getHealth() <= 0)
       {
              gamestate = gameover;
       }
       if((gamestate == gameover ||gamestate==win))//&& (GetAsyncKeyState('P') & 0x8000))
       {
              gamestate = title;
			  reloadLevel(10,10);
			  goal = false;
       }
}

 void ColoredCubeApp::reloadLevel(int x, int z)
{
	currentKeys = 0;
	flashLightObject.getBattery();
	//add in the maze reset function
	Dimension d;
	d.x=x;
	d.z = z;
	maze.init(d,mfxWVPVar,mfxWorldVar,md3dDevice);
	maze.build();

	/*maze.setTex(mfxDiffuseMapVar,mfxSpecMapVar,L"brickwork-texture.jpg",L"brickwork-bump-map.jpg");	
	maze.setCeilTex(mfxDiffuseMapVar,mfxSpecMapVar,L"13.free-brick-textures.jpg",L"brickwork-bump-map.jpg");
	maze.setFloorTex(mfxDiffuseMapVar,mfxSpecMapVar,L"carpet_diffuse.jpg",L"brickwork-bump-map.jpg");*/

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