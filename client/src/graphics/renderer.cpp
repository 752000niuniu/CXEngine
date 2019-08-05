#include "Renderer.h"

Renderer2D::Renderer2D()
:m_ObjectList(0),
m_Graphics()
{
    
}

Renderer2D::~Renderer2D()
{
}


void Renderer2D::Render()
{
    m_Graphics.Begin();
    for(Object2D* obj : m_ObjectList)
    {
        if(obj->IsVisible())
        {
            m_Graphics.Draw(*obj);
        }
    }
    m_Graphics.End();
}


void Renderer2D::AddObject(Object2D* obj)
{
    m_ObjectList.push_back(obj);
}


void Renderer2D::RemoveObject(int index)
{

    //m_ObjectList.remove(index);

}

void Renderer2D::RemoveAllObject()
{
	
	m_ObjectList.clear();
}


RenderThread::RenderThread()
{
}

RenderThread::~RenderThread()
{

}




RenderState::RenderState()
{
}

RenderState::~RenderState()
{
}

void RenderState::SetSpriteRenderer()
{
	Reset();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0);

}

void RenderState::SetTextRenderer()
{

}

void RenderState::Reset()
{
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
}

void RenderState::SetModelRenderer()
{
	Reset();
	glEnable(GL_DEPTH_TEST);
}
