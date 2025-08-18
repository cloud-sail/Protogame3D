#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"

//-----------------------------------------------------------------------------------------------
class Clock;
typedef std::vector<Entity*> EntityList;

// ----------------------------------------------------------------------------------------------
class Game
{
public:
	Game();
	~Game();
	void Update();
	void Render() const;

public:
	Clock* m_clock = nullptr;
	bool m_isAttractMode = true;

private:
	void UpdateEntities();
	void RenderEntities() const;
	void DebugRenderEntities() const;
	void RenderUI() const;
	void UpdateCameras();

	void UpdateDeveloperCheats();
	void AdjustForPauseAndTimeDistortion();

	void UpdateAttractMode();
	void RenderAttractMode() const;

	void DebugDrawStartup();
	void DebugDrawUpdate();


private:
	Camera m_screenCamera;


private:
	EntityList m_allEntities;
	Player* m_player = nullptr;
	Prop* m_shiningCube = nullptr;
	Prop* m_sphere = nullptr;

	Prop* m_billboard = nullptr;

	Skybox* m_skybox;
};
