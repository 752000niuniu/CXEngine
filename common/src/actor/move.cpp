#include "move.h"
#include "window.h"
#include "actor.h"
#include "cxmath.h"

MoveHandle::MoveHandle(Actor* actor)
	:m_Actor(actor)
{
	m_bMove = false;
}

MoveHandle::~MoveHandle()
{

}

void MoveHandle::Update()
{
	if(m_bMove){
		float dt = WINDOW_INSTANCE->GetDeltaTime();
		Pos src = m_Actor->GetPos();
		Pos dest = m_Actor->GetMoveToPos();
		float localVelocity = dt * m_Actor->GetVelocity();
		if (m_Actor->GetMoveDestDistSquare(dest) > localVelocity*localVelocity) {
			float degree = m_Actor->GetMoveDestAngle(dest);
			int m_Dir = m_Actor->GetDirByDegree(degree);

			float stepRangeX = std::cos(DegreeToRadian(degree));
			float stepRangeY = std::sin(DegreeToRadian(degree));

			m_Actor->TranslateX(stepRangeX * localVelocity);
			m_Actor->TranslateY(stepRangeY * localVelocity);

			m_Actor->SetDir(m_Dir);
		}
		else {
			m_Actor->SetPos(dest);
			m_bMove = false;
		}
	}
	
}

void MoveHandle::MoveTo(float x, float y)
{
	m_Actor->SetMoveToPos(Pos(x, y));
	m_bMove = true;
}
