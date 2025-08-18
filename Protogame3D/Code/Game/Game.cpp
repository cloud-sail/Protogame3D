#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Skybox.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

//-----------------------------------------------------------------------------------------------
Game::Game()
{
	m_clock = new Clock();

	m_player = new Player(this);
	m_allEntities.push_back(m_player);

	Prop* cube = new Prop(this);
	cube->m_position = Vec3(2.f, 2.f, 0.f);
	cube->m_angularVelocity = EulerAngles(0.f, 30.f, 30.f);
	AddVertsForColoredCube3D(cube->m_vertexes);
	m_allEntities.push_back(cube);

	m_shiningCube = new Prop(this);
	m_shiningCube->m_position = Vec3(-2.f, -2.f, 0.f);
	AddVertsForColoredCube3D(m_shiningCube->m_vertexes);
	m_allEntities.push_back(m_shiningCube);

	m_sphere = new Prop(this);
	m_sphere->m_position = Vec3(10.f, -5.f, 1.f);
	m_sphere->m_angularVelocity = EulerAngles(45.f, 0.f, 0.f);
	m_sphere->m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	AddVertsForSphere3D(m_sphere->m_vertexes, Vec3(), 1.f);
	m_allEntities.push_back(m_sphere);

	Prop* grid = new Prop(this);
	AddVertsForGridXY(grid->m_vertexes, IntVec2(100, 100));
	m_allEntities.push_back(grid);

	//Prop* cylinder = new Prop(this);
	//AddVertsForCylinder3D(cylinder->m_vertexes, Vec3(5.f, 5.f, 5.f), Vec3(3.f, 3.f, 3.f), 2.f, Rgba8::GREEN, AABB2::ZERO_TO_ONE, 16);
	//m_allEntities.push_back(cylinder);

	//Prop* cone = new Prop(this);
	//AddVertsForCone3D(cone->m_vertexes, Vec3(-5.f, -5.f, -5.f), Vec3(-3.f, -3.f, -3.f), 2.f, Rgba8::BLUE, AABB2::ZERO_TO_ONE, 16);
	//m_allEntities.push_back(cone);

	//Prop* arrow = new Prop(this);
	//AddVertsForArrow3D(arrow->m_vertexes, Vec3(15.f, 15.f, 0.f), Vec3(15.f, 15.f, -4.f), 0.1f, Rgba8::YELLOW, 64);
	//m_allEntities.push_back(arrow);

	//BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	//Prop* text = new Prop(this);
	//font->AddVertsForText3DAtOriginXForward(text->m_vertexes, 0.5f, "Hello World!", Rgba8::MAGENTA, 0.5f, Vec2(1.f, 1.f));
	//text->m_texture = &font->GetTexture();
	//text->m_position = Vec3(2.f, -2.f, 0.f);
	//text->m_angularVelocity = EulerAngles(45.f, 0.f, 0.f);
	//m_allEntities.push_back(text);

	Prop* uvCylinder = new Prop(this);
	AddVertsForCylinderZ3D(uvCylinder->m_vertexes, Vec2(5.f, 5.f), FloatRange(5.f, 10.f), 3.f, 16);
	uvCylinder->m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	m_allEntities.push_back(uvCylinder);

	Prop* uvBox = new Prop(this);
	AddVertsForAABB3D(uvBox->m_vertexes, AABB3(Vec3(-5.f, -5.f, -5.f), Vec3(-3.f, -3.5f, -4.f)));
	uvBox->m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	m_allEntities.push_back(uvBox);

	m_billboard = new Prop(this);
	AddVertsForQuad3D(m_billboard->m_vertexes, Vec3(0.f, -0.5f, -0.5f), Vec3(0.f, 0.5f, -0.5f), Vec3(0.f, 0.5f, 0.5f), Vec3(0.f, -0.5f, 0.5f));
	m_billboard->m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	m_billboard->m_position = Vec3(-5.f, -5.f, 5.f);
	m_allEntities.push_back(m_billboard);

	//Prop* plane = new Prop(this);
	//AddVertsForGridPlane3D(plane->m_vertexes, Plane3(Vec3::MakeFromPolarDegrees(45.f, 45.f), 1.f));
	//m_allEntities.push_back(plane);


	DebugDrawStartup();
	//-----------------------------------------------------------------------------------------------
	SkyboxConfig skyboxConfig;
	skyboxConfig.m_defaultRenderer = g_theRenderer;
	skyboxConfig.m_fileName = "Data/Images/skybox_desert.png";
	skyboxConfig.m_type = SkyboxType::CUBE_MAP;
	m_skybox = new Skybox(skyboxConfig);
}

Game::~Game()
{

}

void Game::Update()
{
	if (m_isAttractMode)
	{
		UpdateAttractMode();
		return;
	}

	XboxController const& controller = g_theInput->GetController(0);
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESCAPE) || 
		controller.WasButtonJustPressed(XboxButtonId::XBOX_BUTTON_BACK))
	{
		m_isAttractMode = true;
	}
	UpdateDeveloperCheats();
	UpdateEntities();
	UpdateCameras();

	DebugDrawUpdate();
}

void Game::Render() const
{
	if (m_isAttractMode)
	{
		RenderAttractMode();
		return;
	}

	// World-space drawing
	g_theRenderer->BeginCamera(m_player->m_camera);
	m_skybox->Render(m_player->m_camera);

	RenderEntities();
	DebugRenderEntities();
	g_theRenderer->EndCamera(m_player->m_camera);
	DebugRenderWorld(m_player->m_camera);

	g_theRenderer->BeginCamera(m_screenCamera);
	RenderUI();
	g_theRenderer->EndCamera(m_screenCamera);
	DebugRenderScreen(m_screenCamera);
}

void Game::UpdateEntities()
{
	float deltaSeconds = static_cast<float>(m_clock->GetDeltaSeconds());
	for (int entityIndex = 0; entityIndex < (int)m_allEntities.size(); ++entityIndex)
	{
		Entity* entity = m_allEntities[entityIndex];

		entity->Update(deltaSeconds);
	}
	// Update special props
	unsigned char brightness = DenormalizeByte(RangeMapClamped(SinRadians((float)m_clock->GetTotalSeconds()), -1.f, 1.f, 0.f, 1.f));
	m_shiningCube->m_color = Rgba8(brightness, brightness, brightness);

	Mat44 billBoardTransform = GetBillboardTransform(BillboardType::WORLD_UP_OPPOSING,
		m_player->GetModelToWorldTransform(),
		m_billboard->m_position);

	m_billboard->m_orientation = billBoardTransform.GetEulerAngles();
	// not sure the case when the scale is negative
	m_billboard->m_scale = Vec3(billBoardTransform.GetIBasis3D().GetLength(), billBoardTransform.GetJBasis3D().GetLength(), billBoardTransform.GetKBasis3D().GetLength());
	m_billboard->m_position = billBoardTransform.GetTranslation3D();
}

void Game::RenderEntities() const
{
	g_theRenderer->BeginRenderEvent("Entities");
	for (int entityIndex = 0; entityIndex < (int)m_allEntities.size(); ++entityIndex)
	{
		Entity* entity = m_allEntities[entityIndex];
		entity->Render();
	}
	g_theRenderer->EndRenderEvent("Entities");
}

void Game::DebugRenderEntities() const
{
	if (!g_isDebugDraw)
	{
		return;
	}
}

void Game::RenderUI() const
{
}

void Game::UpdateCameras()
{
	// Screen camera (for UI, HUD, Attract, etc.)
	m_screenCamera.SetOrthographicView(Vec2(0.f, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));
}


void Game::UpdateDeveloperCheats()
{
	AdjustForPauseAndTimeDistortion();
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		g_isDebugDraw = !g_isDebugDraw;
	}
}


void Game::AdjustForPauseAndTimeDistortion()
{
	if (g_theInput->WasKeyJustPressed(KEYCODE_P))
	{
		m_clock->TogglePause();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_O))
	{
		m_clock->StepSingleFrame();
	}
	
	bool isSlowMo = g_theInput->IsKeyDown(KEYCODE_T);

	m_clock->SetTimeScale(isSlowMo ? 0.1 : 1.0);
}


void Game::UpdateAttractMode()
{
	// Update screenCamera in attract mode
	m_screenCamera.SetOrthographicView(Vec2(0.f, 0.f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));

	XboxController const& controller = g_theInput->GetController(0);
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESCAPE) ||
		controller.WasButtonJustPressed(XboxButtonId::XBOX_BUTTON_BACK))
	{
		g_theApp->HandleQuitRequested();
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_SPACE) ||
		controller.WasButtonJustPressed(XboxButtonId::XBOX_BUTTON_A) ||
		controller.WasButtonJustPressed(XboxButtonId::XBOX_BUTTON_START))
	{
		m_isAttractMode = false;
		return;
	}

}

void Game::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	std::vector<Vertex_PCU> verts;
	AddVertsForRing2D(verts, Vec2(800.f, 400.f), 300.f, 10.f, Rgba8(255, 127, 0));

#ifdef ENGINE_RENDER_D3D11
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
#endif // ENGINE_RENDER_D3D11

#ifdef ENGINE_RENDER_D3D12
	// resource settings
	UnlitRenderResources resources;
	resources.diffuseTextureIndex = g_theRenderer->GetSrvIndexFromLoadedTexture(nullptr, DefaultTexture::WhiteOpaque2D);
	resources.diffuseSamplerIndex = g_theRenderer->GetDefaultSamplerIndex(SamplerMode::POINT_CLAMP);
	resources.cameraConstantsIndex = g_theRenderer->GetCurrentCameraConstantsIndex();
	resources.modelConstantsIndex = g_theRenderer->GetCurrentModelConstantsIndex();

	g_theRenderer->SetGraphicsBindlessResources(sizeof(UnlitRenderResources), &resources);
#endif // ENGINE_RENDER_D3D12

	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRenderTargetFormats();

	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(m_screenCamera);
}

//-----------------------------------------------------------------------------------------------
void Game::DebugDrawStartup()
{
	constexpr float CELL_ASPECT = 0.9f;
	constexpr float TEXT_HEIGHT = 0.2f;
	constexpr float ORIGIN_OFFSET = 0.15f;
	DebugAddWorldBasis(Mat44(), -1.f);
	DebugAddWorldText("x - forward", Mat44(Vec3(0.f, -1.f, 0.f), Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(ORIGIN_OFFSET, 0.f, ORIGIN_OFFSET)), TEXT_HEIGHT, -1.f, CELL_ASPECT, Vec2::ZERO, Rgba8::RED);
	DebugAddWorldText("y - left", Mat44(Vec3(-1.f, 0.f, 0.f), Vec3(0.f, -1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, ORIGIN_OFFSET, ORIGIN_OFFSET)), TEXT_HEIGHT, -1.f, CELL_ASPECT, Vec2(1.f, 0.f), Rgba8::GREEN);
	DebugAddWorldText("z - up", Mat44(Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, -ORIGIN_OFFSET, ORIGIN_OFFSET)), TEXT_HEIGHT, -1.f, CELL_ASPECT, Vec2(0.f, 1.f), Rgba8::BLUE);
}

void Game::DebugDrawUpdate()
{
	// Game Clock Data
	float totalSeconds = (float)m_clock->GetTotalSeconds();
	float frameRate = (float)m_clock->GetFrameRate();
	float timeScale = (float)m_clock->GetTimeScale();

	if (frameRate == 0.f)
	{
		DebugAddScreenText(Stringf("Time: %.2f FPS: inf Scale: %.2f", totalSeconds, timeScale),
			AABB2(Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y) * 0.65f, Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y) * 0.99f),
			15.f, Vec2(1.f, 1.f), 0.f);
	}
	else
	{
		DebugAddScreenText(Stringf("Time: %.2f FPS: %8.02f Scale: %.2f", totalSeconds, frameRate, timeScale),
			AABB2(Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y) * 0.65f, Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y) * 0.99f),
			15.f, Vec2(1.f, 1.f), 0.f);
	}

	// Current Player Position
	Vec3 playerPos = m_player->m_position;
	DebugAddMessage(Stringf("Player Position: %.2f, %.2f, %.2f", playerPos.x, playerPos.y, playerPos.z), 0.f);

	EulerAngles playerOrientation = m_player->m_orientation;
	Vec3 forward, left, up;
	playerOrientation.GetAsVectors_IFwd_JLeft_KUp(forward, left, up);

	if (g_theInput->WasKeyJustPressed('1'))
	{
		DebugAddWorldCylinder(playerPos, playerPos + 20.f * forward, 0.0625, 10.f, Rgba8::YELLOW, Rgba8::YELLOW, DebugRenderMode::X_RAY);
	}

	if (g_theInput->IsKeyDown('2'))
	{
		DebugAddWorldSphere(Vec3(playerPos.x, playerPos.y, 0.f), 0.2f, 60.f, Rgba8(150, 75,0), Rgba8(150, 75, 0));
	}

	if (g_theInput->WasKeyJustPressed('3'))
	{
		DebugAddWorldWireSphere(playerPos + 2.f * forward, 1.f, 5.f, Rgba8::GREEN, Rgba8::RED);
	}

	if (g_theInput->WasKeyJustPressed('4'))
	{
		DebugAddWorldBasis(m_player->GetModelToWorldTransform(), 20.f);
	}

	if (g_theInput->WasKeyJustPressed('5'))
	{
		DebugAddWorldBillboardText(Stringf("Position: %.1f, %.1f, %.1f Orientation: %.1f, %.1f, %.1f", playerPos.x, playerPos.y, playerPos.z, playerOrientation.m_yawDegrees, playerOrientation.m_pitchDegrees, playerOrientation.m_rollDegrees),
			playerPos + 2.f * forward, 0.125f, 10.f, 1.f, Vec2(0.5f, 0.5f), Rgba8::OPAQUE_WHITE, Rgba8::RED);
	}

	if (g_theInput->WasKeyJustPressed('6'))
	{
		DebugAddWorldWireCylinder(playerPos, playerPos + Vec3::ZAXIS, 0.5f, 10.f, Rgba8::OPAQUE_WHITE, Rgba8::RED);
	}

	if (g_theInput->WasKeyJustPressed('7'))
	{
		DebugAddMessage(Stringf("Camera orientation: %.2f, %.2f, %.2f", playerOrientation.m_yawDegrees, playerOrientation.m_pitchDegrees, playerOrientation.m_rollDegrees), 5.f);
	}

	//for (int entityIndex = 0; entityIndex < (int)m_allEntities.size(); ++entityIndex)
	//{
	//	Entity* entity = m_allEntities[entityIndex];
	//	Vec2 viewportPos;
	//	bool isInView = m_player->m_camera.ProjectWorldToViewportPoint(entity->m_position, viewportPos);
	//	if (isInView)
	//	{
	//		AABB2 cameraBound = AABB2(m_screenCamera.GetOrthographicBottomLeft(), m_screenCamera.GetOrthographicTopRight());
	//		Vec2 pointInCamera = cameraBound.GetPointAtUV(viewportPos);
	//		Vec2 halfDimensions = Vec2(20.f, 20.f);
	//		DebugAddScreenText("TAG", AABB2(pointInCamera - halfDimensions, pointInCamera + halfDimensions), 20.f, Vec2(0.5f, 0.5f), 0.f);

	//	}

	//	Vec2 screenPos;
	//	isInView = m_player->m_camera.ProjectWorldToViewportPoint(entity->m_position, screenPos);
	//	if (isInView)
	//	{
	//		Vec2 halfDimensions = Vec2(20.f, 20.f);
	//		DebugAddScreenText("TAG", AABB2(screenPos - halfDimensions, screenPos + halfDimensions), 20.f, Vec2(0.5f, 0.5f), 0.f);
	//	}
	//}

}
