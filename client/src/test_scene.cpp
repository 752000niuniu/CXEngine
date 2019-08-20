#include "test_scene.h"
#include "file_system.h"
#include "vertex.h"
#include "camera.h"
#include "window.h"
#include "input_manager.h"
//#include "model.h"
#include "graphics/Renderer.h"
#include "sprite_renderer.h"
#include "texture_manager.h"
#include "cxrandom.h"
#include "cxmath.h"
#include "graphics/bitmap.h"
#include "scene/scene_manager.h"
#include "net.h"
#include <cctype>
#include "animation/sprite.h"
//
//struct TurtorialAssimp
//{
//	Shader* m_Shader;
//	Model* m_Model;
//	Camera* m_Camera;
//	glm::vec3 lightPos;
//
//	unsigned int VBO, cubeVAO;
//
//	unsigned int lightVAO;
//	std::vector<float> vertices;
//
//	TurtorialAssimp()
//	{
//		RENDER_STATE_INSTANCE->SetModelRenderer();
//
//		m_Shader = new Shader("1.model_loading.vs", "1.model_loading.fs");
//		m_Shader->Bind();
//	
//		m_Model = new Model(FileSystem::GetResourcePath("objects/nanosuit/nanosuit.obj"));
//		m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
//		INPUT_MANAGER_INSTANCE->SetCamera(m_Camera);
//
//	
//		m_Shader->Unbind();
//	}
//	void Draw()
//	{
//		m_Shader->Bind();
//
//		// view/projection transformations
//		glm::mat4 projection = glm::perspective(glm::radians(m_Camera->Zoom), (float)800 / (float)600.f, 0.1f, 100.0f);
//		glm::mat4 view = m_Camera->GetViewMatrix();
//		m_Shader->SetMat4("projection", projection);
//		m_Shader->SetMat4("view", view);
//
//		// render the loaded model
//		glm::mat4 model;
//		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
//		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
//		m_Shader->SetMat4("model", model);
//
//		m_Model->Draw(m_Shader);
//	}
//};
//TurtorialAssimp* assimp;

class Bubble : public View
{
public:
	Bubble(std::string letter = u8"V");
	~Bubble();
	float X;
	float Y;
	float Radius;
	bool Visible;
	std::string  Letter;
	float Velocity;
	float Acceleration;
	float Mass;
	int Dir;
	float Force;
	void Update();
	void Draw();
	bool CheckCollision(const Bubble& bubble);
	void DoCollision(const Bubble& bubble);


	void OnClick(int button, int x, int y) override;
	Bound GetViewBounds()  override;
	int GetViewLayer() const override;
	void OnKeyUpEvent(int keyCode) override;
private:
	Texture* m_Image;
};



Bubble::Bubble(std::string letter)
	:X(0.f),
	Y(0.f),
	Radius(20.f),
	Visible(true),
	Dir(-1),
	Velocity(0.02f)
{
	Mass = 0.5;	//kg
	Acceleration = 0.f;
	//Velocity = Velocity/100.f;  1Ó¢´ç100ÏñËØ 1ÏñËØ/0.01Ó¢´ç 1Ã×=39Ó¢´ç!=3900ÏñËØ
	Force = 0.f;
	Letter = letter;
	m_Image = new Texture(FileSystem::GetAssetsPath("fish.png"), true);
	INPUT_MANAGER_INSTANCE->RegisterView(this);
}


Bubble::~Bubble()
{
	INPUT_MANAGER_INSTANCE->UnRegisterView(this);
	delete m_Image;
	m_Image = nullptr;
}


float accum_time = 0.f;
void Bubble::Update()
{
	//if (Dir >= 0)
	//{
	//	if (X + Radius * 2  > WINDOW_INSTANCE->GetWidth() || Y + Radius * 2> WINDOW_INSTANCE->GetHeight() || X < 0 || Y < 0)
	//	{
	//		Dir += 90;
	//		if (Dir >= 360)
	//			Dir = Dir % 360;
	//		if (X < 0)
	//			X = 0;
	//		if (Y < 0)
	//			Y = 0;
	//		if (X + Radius * 2 > WINDOW_INSTANCE->GetWidth())
	//			X = WINDOW_INSTANCE->GetWidth() - Radius * 2;
	//		if (Y + Radius * 2> WINDOW_INSTANCE->GetHeight())
	//			Y = WINDOW_INSTANCE->GetHeight() - Radius * 2;
	//	}

	//	Y += std::sin(Dir)*Velocity;
	//	X += std::cos(Dir)*Velocity;	
	//}
	float dt = WINDOW_INSTANCE->GetDeltaTime(); //dtÃë

	if (Acceleration == 0.f)
	{
		//ÔÈËÙÖ±ÏßÔË¶¯
		//X += (dt * Velocity) * 100.f * pixs_per_cm;
		X += GMath::Meter2Pixel(dt * Velocity);
	}
	else
	{
		Velocity += Acceleration*dt;
		//ÊÜÁ¦ºóF = ma  F/m = a
		X += GMath::Meter2Pixel(Velocity* dt);
		accum_time += dt;
		if (accum_time >= 0.2f)
		{
			Acceleration = 0.f;
			Velocity = 0.02f;
		}
	}
	
	if (X > 400.f)
		X = 100.f;
	if (X <= 100.f)
	{
		X = 100.f;
	}
}

void Bubble::Draw()
{
	if (Visible)
	{
	//	SPRITE_RENDERER_INSTANCE->DrawTexture(m_Image, glm::vec2(X, Y), glm::vec2(2 * Radius, 2 * Radius));
		SPRITE_RENDERER_INSTANCE->DrawTexture(m_Image, glm::vec2(X, Y), glm::vec2(60, 24));
		//TextRenderer::GetInstance()->DrawTextC(Letter.c_str(), X + Radius, Y + Radius, TextRenderer::CENTER, 2 * Radius, glm::vec3(255.f, 255.0f, 255.f));
	}
}

bool Bubble::CheckCollision(const Bubble& bubble)
{
	if (X + 2 * Radius < bubble.X || X > bubble.X + 2 * bubble.Radius || Y + 2 * Radius < bubble.Y || Y > bubble.Y + 2 * bubble.Radius)
		return false;
	return true;
}


void Bubble::DoCollision(const Bubble& bubble)
{
	Dir += 90;
	if (Dir >= 360)
		Dir = Dir % 360;
	if (X + 2 * Radius >= bubble.X && X <= bubble.X)
		X = bubble.X - 2 * Radius;
	if (X <= bubble.X + 2 * bubble.Radius && X >= bubble.X)
		X = bubble.X + 2 * bubble.Radius;
	if (Y + 2 * Radius >= bubble.Y && Y <= bubble.Y)
		Y = bubble.Y - 2 * Radius;
	if (Y <= bubble.Y + 2 * bubble.Radius && Y >= bubble.Y)
		Y = bubble.Y + 2 * Radius;
	
}

void Bubble::OnClick(int button, int x, int y)
{
	Visible = false;
}

Bound Bubble::GetViewBounds() 
{
	return { X,X + Radius * 2,Y,(Y + Radius * 2) };
}

int Bubble::GetViewLayer() const
{
	return 0;
}

void Bubble::OnKeyUpEvent(int keyCode)
{
	int code = std::toupper( Letter[0]);

	if (keyCode == (GLFW_KEY_A + code - 'A'))
	{
		//Visible = false;
		Force = -0.03f;
		Acceleration = Force / Mass;
		accum_time = 0.f;
	}
}

TestScene::TestScene(int id, String name)
	:BaseScene(id, name)
{

}

TestScene::~TestScene()
{

}

std::vector<Bubble*> bubbleSet;
Texture* p_Bar;
void TestScene::OnLoad()
{
	//assimp = new TurtorialAssimp();
	//auto& files = FileSystem::ListFiles(FileSystem::GetAbsPath("resource\\tables"));
	for (int i = 0; i < 1; i++)
	{
		char letter = 'A' + i;

		Bubble* p_Bubble = new Bubble(  );
		p_Bubble->Letter = letter;
	//	p_Bubble->X = RANDOM_INSTANCE->NextInt(0,WINDOW_INSTANCE->GetWidth());
	//	p_Bubble->Y = RANDOM_INSTANCE->NextInt(0, WINDOW_INSTANCE->GetHeight());
		p_Bubble->X = 100;
		p_Bubble->Y = 200;
		p_Bubble->Dir = RANDOM_INSTANCE->NextInt(0, 359);
		bubbleSet.push_back(p_Bubble);
	}
	//0x80C1B6FF
	//Bitmap::CreateBitmapFile(FileSystem::GetAbsPath("background.bmp"),300.f,4.f,0X80FFFFFF,32) ;
	p_Bar = new Texture(FileSystem::GetResourcePath("background.bmp"), true);
}

void TestScene::OnUnLoad()
{

}

void TestScene::Reset()
{

}

void TestScene::Update()
{
	for (size_t i = 0; i < bubbleSet.size(); i++)
	{
		for (size_t j = i+1; j < bubbleSet.size(); j++)
		{
			if (bubbleSet[i]->CheckCollision(*bubbleSet[j]))
			{
			//	bubbleSet[i]->DoCollision(*bubbleSet[j]);
			}
		}
	}
	for (auto* bubble : bubbleSet)
	{
		bubble->Update();
	}
}

void TestScene::Draw()
{
//	assimp->Draw();
	for (auto* bubble : bubbleSet)
	{
		bubble->Draw();
	}
	SPRITE_RENDERER_INSTANCE->DrawBitmap(p_Bar, glm::vec2(120.f, 212.f), glm::vec2(300.f, 4.f), { 1.f,1.f,1.f }, 0.5f);
}



struct TurtorialLight
{
	Shader* s_LightShader;
	Shader* s_LampShader;
	Camera* s_Camera;
	glm::vec3 lightPos;

	unsigned int VBO, cubeVAO;

	unsigned int lightVAO;
	std::vector<float> vertices;

	TurtorialLight()
		:lightPos(1.2f, 1.0f, 2.0f),
		vertices(VERTEX_FACTORY_INSTANCE->CreateCubic())
	{
		s_LightShader = new Shader("1.colors.vs", "1.colors.fs");
		s_LampShader = new Shader("1.lamp.vs", "1.lamp.fs");
		s_Camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		INPUT_MANAGER_INSTANCE->SetCamera(s_Camera);
		glEnable(GL_DEPTH_TEST);


		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}
	void Draw()
	{
		// be sure to activate shader when setting uniforms/drawing objects
		s_LightShader->Bind();
		s_LightShader->SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
		s_LightShader->SetVec3("lightColor", 1.0f, 1.0f, 1.0f);

		float screenWidth = (float)WINDOW_INSTANCE->GetWidth();
		float screenHeight = (float)WINDOW_INSTANCE->GetHeight();
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(s_Camera->Zoom), screenWidth / screenHeight, 0.1f, 100.0f);
		glm::mat4 view = s_Camera->GetViewMatrix();
		s_LightShader->SetMat4("projection", projection);
		s_LightShader->SetMat4("view", view);

		// world transformation
		glm::mat4 model;
		s_LightShader->SetMat4("model", model);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// also draw the lamp object
		s_LampShader->Bind();
		s_LampShader->SetMat4("projection", projection);
		s_LampShader->SetMat4("view", view);
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		s_LampShader->SetMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};

int demo_count = 1* 5;
float demo_speed = 20.f;
float coffient = 20 * 0.064f;
glm::vec2 demo_dir = glm::normalize(glm::vec2(1, 1));
float demo_distance = 600.f;

std::vector<BaseSprite*> spriteExamples;
AnimationEditorScene::AnimationEditorScene(int id, String name)
	:BaseScene(id, name)
{
	/*for (int i = 0; i < demo_count; i++)
	{
		int row = i / 5;
		int col = i % 5;

		auto playerFrameWasID = GAME_INSTANCE->GetActionWasID(ACTOR_TYPE_PLAYER, 9, Action::Walk);
		auto* sprite = new BaseSprite(SHAPEWDF, playerFrameWasID);
		sprite->Pos.x = (float)(col * sprite->Width + 20 * col);
		sprite->Pos.y = (float)(row * sprite->Height + 20 * row);
		sprite->Dir = row;
		sprite->FrameInterval = 0.016f*(col+2);
		spriteExamples.push_back(sprite);
	}
	demo_distance = 0.f;*/
}

AnimationEditorScene::~AnimationEditorScene() {

}

void AnimationEditorScene::Update() {
	/*float dt = WINDOW_INSTANCE->GetDeltaTime();

	for (int i = 0; i < demo_count; i++)
	{
		glm::vec2 pos(spriteExamples[i]->Pos.x, spriteExamples[i]->Pos.y);
		glm::vec2 dtpos = demo_dir * (coffient/ spriteExamples[i]->FrameInterval) *dt;
		if (i == 0) {
			demo_distance += glm::length(dtpos);
		}
		pos = pos + dtpos;
		spriteExamples[i]->Pos.x = pos.x;
		spriteExamples[i]->Pos.y = pos.y;
		spriteExamples[i]->Update();
	}

	if (demo_distance >= 300.f) {
		demo_distance = 0.f;
		for (int i = 0; i < demo_count; i++)
		{
			int row = i / 5;
			int col = i % 5;
			auto* sprite = spriteExamples[i];
			spriteExamples[i]->Pos.x = (float)(col * sprite->Width + 20 * col);
			spriteExamples[i]->Pos.y = (float)(row * sprite->Height + 20 * row);
		}

	}*/
};

void AnimationEditorScene::Draw() {
	//for (int i = 0; i < demo_count; i++)
	//{
	//	spriteExamples[i]->Draw();
	//}
};
void AnimationEditorScene::Reset() {}




