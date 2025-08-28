#pragma once
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
class AudioSystem;
class InputSystem;
class Renderer;
class Window;
class App;
class Game;
class Clock;
class Skybox;

struct Vertex_PCU;
struct IntVec2;

//-----------------------------------------------------------------------------------------------
class Entity;
class Prop;
class SpectatorCamera;


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
// Gameplay Constants
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;

//-----------------------------------------------------------------------------------------------
void AddVertsForColoredCube3D(std::vector<Vertex_PCU>& verts);

