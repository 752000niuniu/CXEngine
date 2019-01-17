/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "global.h"
#include "Window.h"
#include "file_system.h"
#include "core/Renderer.h"


SpriteRenderer::SpriteRenderer()
{
	// Configure shaders
	RENDER_STATE_INSTANCE->SetSpriteRenderer();

    m_pShader = new Shader("sprite.vs", "sprite.fs");
    m_pShader->Bind();
	glUniform1i(glGetUniformLocation(m_pShader->GetProgramID(), "image"), 0);
	glm::mat4 projection = glm::ortho(0.0f, WINDOW_INSTANCE->GetWidth() *1.0f, WINDOW_INSTANCE->GetHeight()*1.0f, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "projection"), 1, GL_FALSE, (GLfloat*)(&projection));
	glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"), 1.0f);

    initRenderData();
	m_pShader->Unbind();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
	if (m_pShader)
	{
		glDeleteProgram(m_pShader->GetProgramID());
		delete m_pShader;
		m_pShader = nullptr;
	}
}



void SpriteRenderer::DrawTexture(Texture* texture, glm::vec2 position, glm::vec2 size /*= glm::vec2(0, 0)*/, GLfloat rotate /*= 0.0f*/, glm::vec3 color /*= glm::vec3(1.0f)*/)
{
	if (size.x == 0 &&size.y == 0)
	{
		size.x = texture->GetWidth();
		size.y = texture->GetHeight();
	}

	DrawFrameSprite(texture, position, size);
}

void SpriteRenderer::DrawSprite(Texture* texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    // Prepare transformations
    m_pShader->Bind();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
    
    model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
    
    glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*) (&model));
    
    // Render textured quad
    glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"), 1.f);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0);


    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
	m_pShader->Unbind();
    
}
void SpriteRenderer::DrawFrameSprite(Texture* texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
	// Prepare transformations
	m_pShader->Bind();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*)(&model));

	// Render textured quad
	glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"), 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	m_pShader->Unbind();
}


void SpriteRenderer::DrawBitmap(Texture* texture, glm::vec2 position, glm::vec2 size, glm::vec3 color , GLfloat alpha)
{
	m_pShader->Bind();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*)(&model));

	// Render textured quad
	glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), color.x,color.y,color.z );
	glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"), alpha);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	m_pShader->Unbind();
}


void SpriteRenderer::DrawBitmapSymmetrical(Texture* texture, glm::vec2 position, glm::vec2 size /*= glm::vec2(1, 1)*/, glm::vec3 color /*= glm::vec3(1.0f)*/, GLfloat alpha /*= 0.0f*/)
{
	m_pShader->Bind();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*)(&model));

	// Render textured quad
	glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"), alpha);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

	glBindVertexArray(this->symmetricalQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	m_pShader->Unbind();
}

void SpriteRenderer::DrawMask(Texture* texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    // Prepare transformations
    m_pShader->Bind();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
    
    model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
    
    glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*) (&model));
       
    // Render textured quad
    glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), color.x, color.y,0);
	//glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), 0.5f, color.y, 0);
    glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"), 1.f);
    
    glStencilMask(0xFF);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    
    glBlendFuncSeparate(GL_ZERO,GL_ONE,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glStencilMask(0x00);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_pShader->Unbind();
}



void SpriteRenderer::DrawMapSprite(Texture* texture, glm::vec2 position, glm::vec2 size, GLfloat rotate , GLfloat alpha)
{
    // Prepare transformations
    m_pShader->Bind();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
    model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
    
    glUniformMatrix4fv(glGetUniformLocation(m_pShader->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*) (&model));
    
    // Render textured quad
    glUniform3f(glGetUniformLocation(m_pShader->GetProgramID(), "spriteColor"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(m_pShader->GetProgramID(), "alpha"),alpha);

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    
    // glBlendFuncSeparate(GL_ONE,GL_ZERO, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFuncSeparate(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA ,GL_ZERO,GL_ONE);
    glBlendFunc(GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA);

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
	m_pShader->Unbind();
    
}



void SpriteRenderer::initRenderData()
{
    // Configure VAO/VBO
    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


	GLfloat symmetricalVertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f,
		

		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f
	};

	glGenVertexArrays(1, &this->symmetricalQuadVAO);
	glGenBuffers(1, &symmetricalVBO);

	glBindBuffer(GL_ARRAY_BUFFER, symmetricalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(symmetricalVertices), symmetricalVertices, GL_STATIC_DRAW);

	glBindVertexArray(this->symmetricalQuadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void sprite_renderer_init()
{
	SPRITE_RENDERER_INSTANCE;
}


void luaopen_sprite_renderer(lua_State* L)
{
	script_system_register_function(L, sprite_renderer_init);

}