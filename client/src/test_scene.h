#pragma once
#include "scene/BaseScene.h"

class TestScene : public BaseScene
{
public:

	TestScene(int id, String name);
	virtual ~TestScene();
	void OnLoad() override;
	void OnUnLoad() override;
	void Update() override;
	void Draw() override;
	void Reset();

	
};

class TestNetScene : public BaseScene
{
public:
	TestNetScene(int id, String name);
	virtual ~TestNetScene();
	void OnLoad() override {};
	void OnUnLoad() override {};
	void Update() override;
	void Draw() override;
	void Reset();

};



class AnimationEditorScene : public BaseScene
{
public:
	AnimationEditorScene(int id, String name);
	virtual ~AnimationEditorScene();
	void OnLoad() override {};
	void OnUnLoad() override {};
	void Update() override;
	void Draw() override;
	void Reset();

};



