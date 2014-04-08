#ifndef Constants_H
#define Constants_H

//macros from kelly code
//-----------------------------------------------
// Useful macros
//-----------------------------------------------
// Safely delete pointer referenced item
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }
// Safely release pointer referenced item
#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }
// Safely delete pointer referenced array
#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete [](ptr); (ptr)=NULL; } }
// Safely call onLostDevice
#define SAFE_ON_LOST_DEVICE(ptr)    { if(ptr) { ptr->onLostDevice(); } }
// Safely call onResetDevice
#define SAFE_ON_RESET_DEVICE(ptr)   { if(ptr) { ptr->onResetDevice(); } }
#define TRANSCOLOR  SETCOLOR_ARGB(0,255,0,255)  // transparent color (magenta)

const UINT GAME_WIDTH =  1000;              // width of game in pixels
const UINT GAME_HEIGHT = 600;               // height of game in pixels

//Defines to make life easier
#define Vector3 D3DXVECTOR3
#define Vector4 D3DXVECTOR4
#define Vector2 D3DXVECTOR2
#define Matrix D3DXMATRIX
#define Identity D3DXMatrixIdentity
#define Translate D3DXMatrixTranslation
#define RotateX D3DXMatrixRotationX
#define RotateY D3DXMatrixRotationY
#define RotateZ D3DXMatrixRotationZ
#define Scale D3DXMatrixScaling
#define ToRadian D3DXToRadian
#define ToDegree D3DXToDegree
#define Normalize D3DXVec3Normalize
#define VectorZero Vector3(0,0,0)
#define Cross D3DXVec3Cross
#define Dot D3DXVec3Dot
#define Length D3DXVec3Length
#define Dot2 D3DXVec2Dot
#define Length2 D3DXVec2Length

//game states
enum Gamestates {title, controls,level1, level2, gameover, win};

//audio stuff
//C:\Users\GEARHARTJJ1\Documents\College\Junior year\Spring\Games 2\Game Jam 1 Gearhart Young
const char WAVE_BANK[] = "audio\\win\\Wave Bank.xwb";
const char SOUND_BANK[] = "audio\\win\\Sound Bank.xsb";
//sound cues
const char MUSIC[] = "Penumbra";
const char WIN[] = "You Win";
const char BATTERY_DIE[] = "Battery Dying";
const char BATTERY_CHARGE[] = "Battery Charging";
const char P_HIT[] = "Ghost Hits You";
const char G_HIT[] = "Ghost Getting Hit";
const char ITEM[] = "Pick Up a Powerup";
const char WILHELM[] = "WilhelmScream";
const char LOSE[] = "You Lost";

#endif