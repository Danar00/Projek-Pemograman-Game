#ifndef DEMO_H
#define DEMO_H


#include <SOIL/SOIL.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

#define NUM_FRAMES 8
#define FRAME_DUR 80
#define OBJECTNUMBER 10

using namespace glm;

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Render();
	
	float frame_width[OBJECTNUMBER], frame_height[OBJECTNUMBER], frameNumber[OBJECTNUMBER], frame_width2 = 0, frame_height2 = 0,score=0;
private:
	float frame_dur[OBJECTNUMBER], oldxpos[OBJECTNUMBER],oldypos[OBJECTNUMBER], xpos[OBJECTNUMBER], ypos[OBJECTNUMBER], xpos2 = 0, ypos2 = 0, gravity = 0, xVelocity[OBJECTNUMBER], yVelocity[OBJECTNUMBER], yposGround = 0, bgControl = 0;
	GLuint VBO[OBJECTNUMBER], VAO[OBJECTNUMBER], EBO[OBJECTNUMBER], texture[OBJECTNUMBER], program[OBJECTNUMBER], VBO2, VAO2, EBO2, texture2, program2,program3;
	bool walk_anim = false, onGround = true;
	unsigned int frame_idx = 0, flip = 0;
	void Instantiate(float x, float y, int i, char* vertex, char* fragment, char* sprite,float frameNumberH, float frameNumberV);
	void BuildCrateSprite();
	void DrawSprite(int i);
	void DrawCrateSprite();
	bool IsCollided(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);
	void DrawPlayerSprite();
	void UpdatePlayerSpriteAnim(float deltaTime);
	void UpdateBackground(float deltaTime);
	void UpdateObstacle(float deltaTime, int i, bool randomized);
	void ControlPlayerSprite(float deltaTime);
	void InputMap();
};
#endif

