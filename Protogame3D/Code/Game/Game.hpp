#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"

// ----------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();
	void Update();
	void Render() const;

	void OnWindowResized(); // Event WINDOW_RESIZE_EVENT, refresh the setting of the camera
	void UpdateDeveloperCheats();
	CursorMode GetCursorMode() const { return m_cursorMode; }

protected:
	Clock* m_clock = nullptr;
	CursorMode m_cursorMode = CursorMode::POINTER;

	SpectatorCamera* m_spectator = nullptr;
	Camera m_screenCamera;

#pragma region optional
// Attract Mode
private:
	void UpdateAttractMode();
	void RenderAttractMode() const;

	bool m_isAttractMode = true;

// Entity, Prop, Player
private:
	void InitializeEntities();
	void UpdateEntities();
	void RenderEntities() const;

	void UpdateCameras();

	void DebugDrawStartup();
	void DebugDrawUpdate();

private:
	std::vector<Entity*> m_allEntities;
	Prop* m_shiningCube = nullptr;
	Prop* m_sphere = nullptr;

	Prop* m_billboard = nullptr;

	Skybox* m_skybox;
#pragma endregion
};
