#include "Engine.h"	
#include "core/Texture.h"
#include "sprite_batch.h"
#include "cube_batch.h"
#include "input_manager.h"
#include "script_system.h"
#include "scene/scene_manager.h"
#include "scene/splash_scene.h"
#include "Window.h"
#include "resource_manager.h"

Engine::Engine()
:m_DeltaTime(0),m_LastTime(0)
{
	
}

Engine::~Engine()
{

}



void Engine::Init()
{
	INPUT_MANAGER_INSTANCE->Init();

	SCENE_MANAGER_INSTANCE->Init();
	
   	TIMER_MANAGER_INTANCE; //init
	
	
	RESOURCE_MANAGER_INSTANCE;

	SPRITE_RENDERER_INSTANCE;
	
	
}

void Engine::Update()
{
	m_DeltaTime = WINDOW_INSTANCE->GetDeltaTime();

	TIMER_MANAGER_INTANCE->Update();

    SCENE_MANAGER_INSTANCE->Update();

}

void Engine::Draw()
{ 	
	SCENE_MANAGER_INSTANCE->Draw();
}

void Engine::Destroy()
{
	INPUT_MANAGER_INSTANCE->DeleteSingleton();
		
	SCENE_MANAGER_INSTANCE->DeleteSingleton();
	
   	TIMER_MANAGER_INTANCE->DeleteSingleton(); 
	
}


