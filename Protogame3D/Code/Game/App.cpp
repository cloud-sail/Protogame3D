#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Math/AABB2.hpp"

#ifdef ENGINE_RENDER_D3D11
#include "Engine/Renderer/DX11Renderer.hpp"
#endif
#ifdef ENGINE_RENDER_D3D12
#include "Engine/Renderer/DX12Renderer.hpp"
#endif

//-----------------------------------------------------------------------------------------------
App*			g_theApp		= nullptr;		// Created and owned by Main_Windows.cpp
Window*			g_theWindow		= nullptr;		// Created and owned by the App
Renderer*		g_theRenderer	= nullptr;		// Created and owned by the App
AudioSystem*    g_theAudio		= nullptr;		// Created and owned by the App
Game*			g_theGame		= nullptr;		// Created and owned by the App
bool			g_isDebugDraw	= false;

//-----------------------------------------------------------------------------------------------
bool OnQuitEvent(EventArgs& args)
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return false;
}


//-----------------------------------------------------------------------------------------------
App::App()
{
}

App::~App()
{
}

void App::Startup()
{
	// Parse Data/GameConfig.xml
	LoadGameConfig("Data/GameConfig.xml");

	// Create all Engine subsystems
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);

	InputConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_inputSystem = g_theInput;
	windowConfig.m_aspectRatio = g_gameConfigBlackboard.GetValue("windowAspect", 1.777f);
#ifdef ENGINE_RENDER_D3D11
	windowConfig.m_windowTitle = g_gameConfigBlackboard.GetValue("windowTitle", "Protogame3D DX11");
#endif
#ifdef ENGINE_RENDER_D3D12
	windowConfig.m_windowTitle = g_gameConfigBlackboard.GetValue("windowTitle", "Protogame3D DX12");
#endif
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
#ifdef ENGINE_RENDER_D3D11
	g_theRenderer = new DX11Renderer(rendererConfig);
#endif
#ifdef ENGINE_RENDER_D3D12
	g_theRenderer = new DX12Renderer(rendererConfig);
#endif

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_defaultRenderer = g_theRenderer;
	devConsoleConfig.m_fontName = g_gameConfigBlackboard.GetValue("fontName", devConsoleConfig.m_fontName);
	devConsoleConfig.m_fontAspectScale = 0.8f;
	g_theDevConsole = new DevConsole(devConsoleConfig);

	AudioConfig audioConfig;
	g_theAudio = new AudioSystem(audioConfig);

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_renderer = g_theRenderer;
	debugRenderConfig.m_messageCellHeight = 15.f;
	debugRenderConfig.m_messageAspectRatio = 0.75f;

	// Start up all Engine subsystems
	g_theEventSystem->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theAudio->Startup();
	DebugRenderSystemStartup(debugRenderConfig);

	// Initialize game-related stuff: create and start the game
	g_theEventSystem->SubscribeEventCallbackFunction("Quit", OnQuitEvent);
	//Clock::TickSystemClock(); // timer created before system clock tick will not start correctly
	g_theGame = new Game();

	g_theDevConsole->AddText(DevConsole::INFO_MAJOR, "Controls");
	g_theDevConsole->AddText(DevConsole::INFO_MINOR, R"(Mouse and KeyBoard:
Mouse  - Aim
W / S  - Move
A / D  - Strafe
Q / E  - Elevate
Z / C  - Roll
Shift  - Sprint
H      - Reset Camera
1      - Spawn Line
2      - Spawn Point
3      - Spawn Wireframe Sphere
4      - Spawn Basis
5      - Spawn Billboarded Text
6      - Spawn Wireframe Cylinder
7      - Add Message
~      - Open Dev Console
Escape - Exit Game
Space  - Start Game

GamePad:
Right Stick  - Aim
Left Stick   - Move
L/R Trigger  - Roll
L/R Shoulder - Elevate
A            - Sprint
Start        - Reset Camera
)");

}

void App::Shutdown()
{
	// Destroy game-related stuff
	delete g_theGame;
	g_theGame = nullptr;

	// Shut down all Engine subsystems
	DebugRenderSystemShutdown();
	g_theAudio->Shutdown();
	g_theRenderer->Shutdown();
	g_theWindow->Shutdown();
	g_theInput->Shutdown();
	g_theDevConsole->Shutdown();
	g_theEventSystem->Shutdown();

	// Destroy all engine subsystems
	delete g_theAudio;
	g_theAudio = nullptr;
	delete g_theDevConsole;
	g_theDevConsole = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theWindow;
	g_theWindow = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete g_theEventSystem;
	g_theEventSystem = nullptr;
}

void App::RunMainLoop()
{
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::RunFrame()
{
	Clock::TickSystemClock();

	BeginFrame();			// Engine pre-frame stuff
	Update();	// Game updates / moves/ spawns / hurts/ kills stuffs
	Render();				// Game draws current state of things
	EndFrame();				// Engine post-frame stuff
}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

void App::BeginFrame()
{
	g_theEventSystem->BeginFrame();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theAudio->BeginFrame();
	DebugRenderBeginFrame();
}

void App::Update()
{
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		delete g_theGame;
		g_theGame = new Game();
	}
	if (!g_theWindow->IsFocused() || g_theDevConsole->GetMode() != DevConsoleMode::HIDDEN || g_theGame->m_isAttractMode)
	{
		g_theInput->SetCursorMode(CursorMode::POINTER);
	}
	else
	{
		g_theInput->SetCursorMode(CursorMode::FPS);
	}

	g_theGame->Update();
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(100, 100, 100));
	g_theGame->Render();

	// Render DevConsole
	Camera devConsoleCamera;
	AABB2 bounds = AABB2(0.f, 0.f, g_theWindow->GetAspectRatio(), 1.f);
	devConsoleCamera.SetOrthographicView(bounds.m_mins, bounds.m_maxs);
	g_theRenderer->BeginCamera(devConsoleCamera);
	g_theDevConsole->Render(bounds);
	g_theRenderer->EndCamera(devConsoleCamera);


}

void App::EndFrame()
{
	DebugRenderEndFrame();
	g_theAudio->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_theInput->EndFrame();
	g_theEventSystem->EndFrame();
}

void App::LoadGameConfig(char const* gameConfigXmlFilePath)
{
	XmlDocument gameConfigXml;
	XmlResult result = gameConfigXml.LoadFile(gameConfigXmlFilePath);
	if (result != tinyxml2::XML_SUCCESS)
	{
		DebuggerPrintf("WARNING: failed to load game config from file \"%s\"\n", gameConfigXmlFilePath);
		return;
	}
	XmlElement* rootElement = gameConfigXml.RootElement();
	if (rootElement == nullptr)
	{
		DebuggerPrintf("WARNING: game config from file \"%s\" was invalid (missing root element)\n", gameConfigXmlFilePath);
		return;
	}
	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*rootElement);
}
