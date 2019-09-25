#include "move.h"
#include "window.h"
#include "actor.h"
#include "cxmath.h"
#include "scene/base_scene.h"
#include "scene/game_map.h"
#include "action.h"

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
	if (!m_Actor->GetProperty(PROP_CAN_MOVE).toBool())return;
	if(m_bMove){
		Pos moveTo = m_Actor->GetMoveToPos();
		Pos pos = m_Actor->GetPos();
		if (moveTo.x != pos.x || moveTo.y != pos.y) {
			float dt = WINDOW_INSTANCE->GetDeltaTime();
			float localVelocity = m_Actor->GetProperty(PROP_MOVE_VELOCITY).toFloat()*dt;
			bool useMoveList = !m_MoveList.empty();
			Pos dest;
			if (useMoveList)
			{
				Pos d = m_MoveList.front();
				dest.x = d.x * 20 + 10;
				dest.y = d.y * 20 + 10;
			}
			else
			{
				dest.x = m_Actor->GetMoveToPos().x;
				dest.y = m_Actor->GetMoveToPos().y;
			}

			if (m_Actor->GetMoveDestDistSquare(dest) > localVelocity*localVelocity) {
				float degree = m_Actor->GetMoveDestAngle(dest);
				int m_Dir = m_Actor->GetDirByDegree(degree);
				float stepRangeX = cos(DegreeToRadian(degree));
				float stepRangeY = sin(DegreeToRadian(degree));
				m_Actor->TranslateX(stepRangeX * localVelocity);
				m_Actor->TranslateY(stepRangeY * localVelocity);
				m_Actor->SetDir(m_Dir);
			}
			else
			{
				if (useMoveList)
				{
					m_MoveList.pop_front();
				}
				m_Actor->SetX(dest.x);
				m_Actor->SetY(dest.y);
			}
		}
		else {
			Pos dest;
			dest.x = m_Actor->GetMoveToPos().x;
			dest.y = m_Actor->GetMoveToPos().y;
			m_Actor->SetPos(dest);
			m_bMove = false;
		}
	}
	
}

void MoveHandle::MoveOnScreen(float x, float y)
{
	m_Actor->SetMoveToPos({ x,y });
	m_bMove = true;
}

void MoveHandle::MoveTo(float x, float y)
{
	if (!m_Actor->GetScene())return;
	if (!m_Actor->GetProperty(PROP_CAN_MOVE).toBool())return;
	GameMap* map = m_Actor->GetScene()->GetGameMap();
	if (!map)return;
	if(m_Actor->GetScene()->IsCombat()){
		m_BackupMoveList.clear();
		m_MoveList.clear();
	}
	else {
		m_BackupMoveList.clear();
		m_BackupMoveList = map->Move(m_Actor->GetBoxX(), m_Actor->GetBoxY(), (int)(x / 20), (int)(y / 20));
		if (m_BackupMoveList.size() < 2)return;
		m_BackupMoveList.pop_front();
		m_BackupMoveList.pop_back();
		m_MoveList = m_BackupMoveList;
	}
	

	m_Actor->SetMoveToPos({ x,y });
	m_bMove = true;
}

