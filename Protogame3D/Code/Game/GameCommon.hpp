#pragma once
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
class AudioSystem;
class InputSystem;
class Renderer;
class Window;
class App;
class Game;
class Entity;

//-----------------------------------------------------------------------------------------------
extern AudioSystem*		g_theAudio;
extern InputSystem*		g_theInput;
extern Renderer*		g_theRenderer;
extern Window*			g_theWindow;
extern App*				g_theApp;
extern Game*			g_theGame;

//-----------------------------------------------------------------------------------------------
extern bool g_isDebugDraw;


//-----------------------------------------------------------------------------------------------
class Skybox;
class Player;
class Prop;

struct Vertex_PCU;
struct IntVec2;

//-----------------------------------------------------------------------------------------------
// Gameplay Constants
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;

constexpr float CAMERA_MOVE_SPEED = 2.f;
constexpr float CAMERA_YAW_TURN_RATE = 60.f;
constexpr float CAMERA_PITCH_TURN_RATE = 60.f;
constexpr float CAMERA_ROLL_TURN_RATE = 90.f;
constexpr float CAMERA_SPEED_FACTOR = 10.f;

constexpr float CAMERA_MAX_PITCH = 85.f;
constexpr float CAMERA_MAX_ROLL = 45.f;

//-----------------------------------------------------------------------------------------------
void AddVertsForColoredCube3D(std::vector<Vertex_PCU>& verts);

