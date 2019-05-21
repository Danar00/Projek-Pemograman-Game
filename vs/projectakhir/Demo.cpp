#include "Demo.h"




Demo::Demo()
{
}


Demo::~Demo()
{
	if (Mix_Playing(sfx_channel) == 0) {
		Mix_FreeChunk(sound);
	}
	if (music != NULL) {
		Mix_FreeMusic(music);
	}
	Mix_CloseAudio();
}

void Demo::Init()
{
	
	InitText(6);
	this->program = BuildShader("shader.vert", "shader.frag");
	this->program3 = BuildShader("shader.vert", "shader.frag");
	isGameOver = false;
	InitAudio();
	Instantiate(0,0,0, "parralax.vert", "parralax.frag", "seamless.png", 1.0f, 1.0f);
	Instantiate(810, 256, 2, "crateSprite.vert", "crateSprite.frag", "obstacle3.png", 1.0f, 1.0f);
	Instantiate(1215, 128, 3, "crateSprite.vert", "crateSprite.frag", "obstacle2.png", 1.0f, 1.0f);
	Instantiate(1100, 400, 4, "crateSprite.vert", "crateSprite.frag", "obstacle1.png", 1.0f, 1.0f);
	Instantiate(0, 0, 5, "crateSprite.vert", "crateSprite.frag", "gameOver.png", 1.0f, 1.0f);
	Instantiate(256, 256, 1, "playerSprite.vert", "playerSprite.frag", "player.png", 1.0f, 1.0f);
	xVelocity[2] = 5;
	xVelocity[3] = 4;
	xVelocity[4] = 3;	  
	
	InputMap();
	Mix_PlayMusic(music, -1);
	SDL_Delay(150);
}


void Demo::ShowText() {
	int a = score/1000;
	std::string scoreText = std::to_string(a);
	RenderText(6, "Score : " + scoreText, 10, 20, 1.0f, vec3(1, 1, 1));
	//RenderText(8, "Poin", 400, 800, 10.0f, vec3(1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f));
//	Instantiate(1100, 400, 10, "crateSprite.vert", "crateSprite.frag", "game over.jpg", 1.0f, 1.0f);
	//DrawSprite(10);
}

void Demo::RenderText(int index,string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	// Activate corresponding render state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UseShader(this->program3);

	glUniform3f(glGetUniformLocation(this->program3, "ourColor"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(this->program3, "text"), 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->program3, "ourTexture"), 0);
	mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "model"), 1, GL_FALSE, value_ptr(model));
	glBindVertexArray(VAO[index]);

	// Iterate through all characters
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character

		GLfloat vertices[] = {
			// Positions   // Texture Coords
			xpos + w,	ypos + h,	1.0f, 1.0f, // Bottom Right
			xpos + w,	ypos,		1.0f, 0.0f, // Top Right
			xpos,		ypos,		0.0f, 0.0f, // Top Left
			xpos,		ypos + h,	0.0f, 1.0f  // Bottom Left 
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_QUADS, 0, 4);
		// Now advance cursors for next glyph (note that advance is number of 1 / 64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels(2 ^ 6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "projection"), 1, GL_FALSE, value_ptr(projection));

}
void Demo::InitText(int index) {
	// Init Freetype
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		cout << "LIBRARY NOT FOUNTD" << endl;
		//Err("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, FONTNAME, 0, &face)) {
		cout << "FONT NOT FOUND" << endl;

		//Err("ERROR::FREETYPE: Failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, FONTSIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO[index]);
	glGenBuffers(1, &VBO[index]);
	glBindVertexArray(VAO[index]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[index]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL,
		GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Demo::Update(float deltaTime)
{

	if (IsKeyDown("Quit")) {
		SDL_Quit();
		exit(0);
	}
	if (IsKeyDown("Restart") && isGameOver) {
		Init();
	}
	if (!isGameOver) {
		score += deltaTime;
	}
	
	
	UpdateBackground(deltaTime);
	if(!isGameOver)
	ControlPlayerSprite(deltaTime);
	UpdateObstacle(deltaTime, 2, true);
	UpdateObstacle(deltaTime, 3, true);
	UpdateObstacle(deltaTime, 4, false);
	for (int i = 2; i <= 4; i++)
	{
		if (IsCollided(xpos[i], ypos[i], frame_width[i], frame_height[i], xpos[1], ypos[1], frame_height[1], frame_height[1])) {
			cout <<  score/1000 << endl;
			isGameOver = true;
		}
	}
	
}

void Demo::Render()
{
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "projection"), 1, GL_FALSE, value_ptr(projection));
	//Set the background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//mat4 projection;
	DrawSprite(0);
	DrawSprite(1);
	DrawSprite(2);
	DrawSprite(3);
	DrawSprite(4);
	ShowText();

	//GameOver();
	if (isGameOver) {
		GameOver();
	}
}

void Demo::GameOver() {
	DrawSprite(5);
	if (Mix_Playing(sfx_channel) == 0) {
		Mix_PauseMusic();
		SDL_Delay(150);
		sfx_channel = Mix_PlayChannel(-1, sound, 0);
		if (sfx_channel == -1) {
			cout << ("Unable to play WAV file: " + string(Mix_GetError())) << endl;
		}
	}
	ShowText();
}

void Demo::UpdateBackground(float deltaTime) {
	bgControl++;
	UseShader(this->program);
	glUniform1f(glGetUniformLocation(this->program, "x"), bgControl);
}

void Demo::UpdateObstacle(float deltaTime, int i, bool randomized) {
	xpos[i] -= xVelocity[i];
	if (xpos[i]<-frame_width[i])
	{
		xpos[i] = 802;
		int range = (int) (600.0f - frame_height[i]);
		if(randomized) 
			ypos[i] = (rand() % range) + 1;
		xVelocity[i]+=0.6f;
	}
}

void Demo::UpdatePlayerSpriteAnim(float deltaTime)
{
	//// Update animation
	//frame_dur[1] += deltaTime;

	//if (walk_anim && frame_dur[1] > FRAME_DUR) {
	//	//frame_dur = 0;
	//	if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

	//	//	// Pass frameIndex to shader
	//	UseShader(this->program);
	//	glUniform1i(glGetUniformLocation(this->program, "frameIndex"), frame_idx);
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
	if (ypos[1] < 0) {
		ypos[1] =0;
	}
	
}

void Demo::DrawSprite(int i) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos[i], ypos[i], 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width[i], frame_height[i], 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

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
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos[0], ypos[0], 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width[0], frame_height[0], 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO[0]);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::Instantiate(float x, float y, int i, char* vertex,char* fragment,char* sprite,float frameNumberH, float frameNumberV)
{
	this->program = BuildShader(vertex, fragment);

	// Pass n to shader
	UseShader(this->program);
	glUniform1f(glGetUniformLocation(this->program, "n"), 1.0f / frameNumberH);
	glUniform1f(glGetUniformLocation(this->program, "m"), 1.0f / frameNumberV);

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
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));

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
	InputMapping("Restart", SDLK_r);
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

void Demo::InitAudio() {
	int flags = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG;
	if (flags != Mix_Init(flags)) {
		cout<<("Unable to initialize mixer: " + string(Mix_GetError()))<<endl;
	}

	int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		cout<<("Unable to initialize audio: " + string(Mix_GetError()))<<endl;
	}


	music = Mix_LoadMUS("bensound-funkyelement.ogg");
	if (music == NULL) {
		cout << ("Unable to load Music file: " + string(Mix_GetError())) << endl;
	}

	sound = Mix_LoadWAV("grsites.com_whirls.wav");
	if (sound == NULL) {
		cout << ("Unable to load WAV file: " + string(Mix_GetError())) << endl;
	}


}

int main(int argc, char** argv) {

	Engine::Game &game = Demo();
	game.Start("Collision Detection using AABB", 800, 600, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
