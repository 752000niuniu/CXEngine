#pragma once
#include "defines.h"

#include "../astar/Heap.h"
#include "../astar/Astar.h"

#include "Pos.h"

/*
游戏地图类的封装
*/
class GameMap  
{
public:
	GameMap(uint32 mapID);
	
	~GameMap();

	std::list<Pos> Move(int sx, int sy, int ex, int ey);

	NE::MAP* GetMapPtr();

	void clamp(int val, int min, int max);

	Astar* GetAstar() { return m_Astar; };

	int GetRowCount() { return m_Row; };

	int GetColCount() { return m_Col; };

	IntPos GetRandomPos();

	void Update();

	int GetWidth(){return m_Width;}

	int GetHeight(){return m_Height;}

	int GetMapWidth() { return m_MapWidth; }

	int GetMapHeight() { return m_MapHeight; }

	int GetMapOffsetX();

	int GetMapOffsetY();


private:
	bool CanArriveDirect(Pos src, Pos dest);

	void UpdateCell();

	int m_Width;

	int m_Height;

	int m_MapWidth;

	int m_MapHeight;

	int m_CellWidth;

	int m_CellHeight;

	/*
	障碍信息
	*/
	std::vector<std::vector<int>> m_Cell;
	
	int m_MapTileWidth;
	
	int m_MapTileHeight;
	
	int m_MapTileCoef;
	
	int m_Row;
	
	int m_Col;
	
	NE::MAP* m_XyqMap;
	
	Astar* m_Astar;
	
	int m_MapID;
	
	int m_MapOffsetX;
	
	int m_MapOffsetY;
	
	std::map<int, std::set<int>> m_UnitMasks;
};


