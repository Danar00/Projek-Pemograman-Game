#include "Demo.h"


Demo::Demo()
{
}


Demo::~Demo()
{
}

void Demo::Init()
{
	Instantiate(0,0,0, "parralax.vert", "parralax.frag", "seamless.png", 1.0f, 1.0f);
	Instantiate(256,256,1, "playerSprite.vert", "playerSprite.frag", "player.png", 1.0f, 1.0f);
	Instantiate(810, 256, 2, "crateSprite.vert", "crateSprite.frag", "obstacle3.png", 1.0f, 1.0f);
	Instantiate(1215, 128, 3, "crateSprite.vert", "crateSprite.frag", "obstacle2.png", 1.0f, 1.0f);
	Instantiate(1100, 400, 4, "crateSprite.vert", "crateSprite.frag", "obstacle1.png", 1.0f, 1.0f);
	xVelocity[2] = 5;
	xVelocity[3] = 4;
	xVelocity[4] = 3;
	
	//BuildCrateSprite();
	InputMap();
}

void Demo::Update(float deltaTime)
{

	if (IsKeyDown("Quit")) {
		SDL_Quit();
		exit(0);
	}
	score += deltaTime;
	
	UpdateBackground(deltaTime);
	ControlPlayerSprite(deltaTime);
	UpdateObstacle(deltaTime, 2, true);
	UpdateObstacle(deltaTime, 3, true);
	UpdateObstacle(deltaTime, 4, false);
	for (int i = 2; i <= 4; i++)
	{
		if (IsCollided(xpos[i], ypos[i], frame_width[i], frame_height[i], xpos[1], ypos[1], frame_height[1], frame_height[1])) {
			cout <<  score/1000 << endl;
		}
	}
	
}



void Demo::Render()
{
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	//DrawPlayerSprite();
	//DrawCrateSprite();
	/*for (size_t i = 0; i < 10; i++)
	{
		DrawSprite(i);

	}*/
	DrawSprite(0);
	DrawSprite(1);
	DrawSprite(2);
	DrawSprite(3);
	DrawSprite(4);
}

void Demo::UpdateBackground(float deltaTime) {
	bgControl++;
	UseShader(this->program[0]);
	glUniform1f(glGetUniformLocation(this->program[0], "x"), bgControl);
}

void Demo::UpdateObstacle(float deltaTime, int i, bool randomized) {
	xpos[i] -= xVelocity[i];
	if (xpos[i]<-frame_width[i])
	{
		xpos[i] = 802;
		int range = (int) (600.0f - frame_height[i]);
		if(randomized) ypos[i] = (rand() % range) + 1;
		xVelocity[i]+=0.6f;
	}
}

void Demo::UpdatePlayerSpriteAnim(float deltaTime)
{
	//// Update animation
	////frame_dur += deltaTime;

	//if (walk_anim && frame_dur > FRAME_DUR) {
	//	//frame_dur = 0;
	//	if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

	//	// Pass frameIndex to shader
	//	UseShader(this->program[0]);
	//	glUniform1i(glGetUniformLocation(this->program[0], "frameIndex"), frame_idx);
	//}
}

void Demo::ControlPlayerSprite(float deltaTime)
{
	bool jump = false;
	walk_anim = false;
	/*oldxpos = xpos;
	oldypos = ypos;*/

	if (IsKeyDown("Move Right")) {
		xpos[1] += deltaTime * xVelocity[1];
		flip = 0;
		walk_anim = true;
	}

	if (IsKeyDown("Move Left")) {
		xpos[1] -= deltaTime * xVelocity[1];
		flip = 1;
		walk_anim = true;
	}

	if (IsKeyDown("Move Up")) {
		ypos[1] -= (deltaTime +2) * yVelocity[1]+3;
	}
	else {
		ypos[1] += deltaTime * yVelocity[1]+2;
	}
	if (IsKeyDown("Move Down")) {
	}
	
}

void Demo::DrawSprite(int i) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	UseShader(this->program[i]);
	glUniform1i(glGetUniformLocation(this->program[i], "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program[i], "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos[i], ypos[i], 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width[i], frame_height[i], 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program[i], "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO[i]);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::DrawPlayerSprite() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	UseShader(this->program[0]);
	glUniform1i(glGetUniformLocation(this->program[0], "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program[0], "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos[0], ypos[0], 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width[0], frame_height[0], 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program[0], "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO[0]);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::Instantiate(float x, float y, int i, char* vertex,char* fragment,char* sprite,float frameNumberH, float frameNumberV)
{
	this->program[i] = BuildShader(vertex, fragment);

	// Pass n to shader
	UseShader(this->program[i]);
	glUniform1f(glGetUniformLocation(this->program[i], "n"), 1.0f / frameNumberH);
	glUniform1f(glGetUniformLocation(this->program[i], "m"), 1.0f / frameNumberV);

	// Load and create a texture 
	glGenTextures(1, &texture[i]);
	glBindTexture(GL_TEXTURE_2D, texture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image(sprite, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width[i] = ((float)width) / frameNumberH;
	frame_height[i] = (float)height / frameNumberV;
	float temp = 1;
	
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO[i]);
	glGenBuffers(1, &VBO[i]);
	glGenBuffers(1, &EBO[i]);

	glBindVertexArray(VAO[i]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program[i], "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos[i] = x;
	yposGround = GetScreenHeight() - frame_height[i];
	ypos[i] = y;
	//gravity = 0.05f;
	xVelocity[i] = 0.1f;
	yVelocity[i] = 0.1f;

}

void Demo::InputMap() {
	InputMapping("Move Right", SDLK_RIGHT);
	InputMapping("Move Left", SDLK_LEFT);
	InputMapping("Move Up", SDLK_UP);
	InputMapping("Move Down", SDLK_DOWN);
	InputMapping("Move Right", SDLK_d);
	InputMapping("Move Left", SDLK_a);
	InputMapping("Move Right", SDL_BUTTON_RIGHT);
	InputMapping("Move Left", SDL_BUTTON_LEFT);
	InputMapping("Move Right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	InputMapping("Move Left", SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	InputMapping("Jump", SDLK_w);
	InputMapping("Quit", SDLK_ESCAPE);
}


void Demo::BuildCrateSprite()
{
	this->program2 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program2);

	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("crate.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width2 = (float)width;
	frame_height2 = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};



	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos2 = (GetScreenWidth() - frame_width2) / 4;
	ypos2 = GetScreenHeight() - frame_height2;
}



void Demo::DrawCrateSprite() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// Activate shader
	UseShader(this->program2);
	glUniform1i(glGetUniformLocation(this->program2, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos2, ypos2, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width2, frame_height2, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO2);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

bool Demo::IsCollided(float x1, float y1, float width1, float height1,
	float x2, float y2, float width2, float height2) {
	return (x1 < x2 + width2 && x1 + width1 > x2 && y1 < y2 + height2 && y1 + height1 > y2);
}


int main(int argc, char** argv) {

	Engine::Game &game = Demo();
	game.Start("Collision Detection using AABB", 800, 600, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
