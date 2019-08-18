#pragma once


class GameMap;
class Actor;

class MoveHandle
{
public:
	MoveHandle(Actor* actor);
	virtual ~MoveHandle();
	void Update();
	void MoveTo(float x, float y);
private:
	bool m_bMove;
	GameMap* m_Map; 
	Actor * m_Actor;
};