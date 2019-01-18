#include "GameMap.h"

#include <functional>
#include <thread>
#include "../file_system.h"
#include "cxmath.h"
#include "cxrandom.h"
#include <stdint.h>

GameMap::GameMap(uint32_t mapID)
	:m_XyqMap(nullptr)
{
	m_MapID = mapID;

	std::string fileName = FileSystem::GetMapPath(std::to_string(mapID));

	m_Astar = nullptr;

	m_MapOffsetX = m_MapOffsetY = 0;

	m_MapTileWidth = 320;
	m_MapTileHeight = 240;

	//这才是资源
	m_XyqMap = new NE::MAP(fileName);

	m_MapWidth = m_XyqMap->MapWidth();

	m_MapHeight = m_XyqMap->MapHeight();

	m_Width = m_XyqMap->SliceWidth();
	m_Height = m_XyqMap->SliceHeight();

	m_Row = m_XyqMap->Row();
	m_Col = m_XyqMap->Col();
	printf("初始化GameMap %d %d ", m_Row, m_Col);

	m_CellWidth = m_Col * 16;
	m_CellHeight = m_Row * 12;
	m_Cell.resize(m_CellWidth, { 0 });

	for (int i = 0; i < m_CellWidth; i++)
	{
		m_Cell[i].resize(m_CellHeight, 0);
	}
	UpdateCell();


}

void GameMap::clamp(int val, int min, int max)
{
	if (val < min) val = min;
	if (val > max) val = max;
}

NE::MAP* GameMap::GetMapPtr()
{
	return m_XyqMap;
}

bool GameMap::CanArriveDirect(Pos src, Pos dest)
{
	int dx = static_cast<int> (dest.x - src.x);
	int dy = static_cast<int>(dest.y - src.y);
	if (dx == 0)
	{
		int opt = dy < 0 ? -1 : 1;
		for (int i = 0; i<dy*opt; i++)
		{
			int cellX = static_cast<int>(src.x);
			int cellY = static_cast<int>(src.y + i * opt);
			if (m_Cell[cellX][cellY] >= 1)
				return false;
		}
		return true;
	}

	if (dy == 0)
	{
		int opt = dx < 0 ? -1 : 1;
		for (int i = 0; i<dx*opt; i++)
		{
			int cellX = static_cast<int>(src.x + i * opt);
			int cellY = static_cast<int>(src.y);
			if (m_Cell[cellX][cellY] >= 1)
				return false;
		}
		return true;
	}

	double ratio = dy / dx;
	int opt = dx < 0 ? -1 : 1;
	for (int i = 0; i < dx * opt; ++i)
	{
		int cellX = static_cast<int>(src.x + opt * i);
		int cellY = static_cast<int>(src.y + ratio * i*opt);
		if (m_Cell[cellX][cellY] >= 1)
			return false;
	}

	ratio = dx / dy;
	opt = dy < 0 ? -1 : 1;
	for (int i = 0; i < dy * opt; ++i)
	{
		int cellX = static_cast<int>(src.x + ratio * opt*i);
		int cellY = static_cast<int>(src.y + i * opt);
		if (m_Cell[cellX][cellY] >= 1)
			return false;
	}

	return true;

}
void GameMap::UpdateCell()
{
	int x = 0, y = 0, p, q;
	for (int i = 0; i < m_Row; i++)
	{
		for (int j = 0; j < m_Col; j++)
		{
			int index = i * m_Col + j;
			y = 12 * i;
			for (p = 0; p < 12; p++)
			{
				x = 16 * j;
				for (q = 0; q < 16; q++)
				{
					m_Cell[x++][y] = m_XyqMap->GetUnit(i*m_Col + j).Cell[p * 16 + q];
				}
				y++;
			}
		}
	}
	m_Astar = new Astar(m_MapWidth, m_MapHeight, m_Cell);
}

IntPos GameMap::GetRandomPos()
{
	IntPos dest;
	do
	{
		dest.x = Random::GetInstance()->NextInt(0, m_CellWidth - 1);
		dest.y = Random::GetInstance()->NextInt(0, m_CellHeight - 1);
	} while (m_Cell[dest.x][dest.y] >= 1);
	return dest;
}

std::list<Pos> GameMap::Move(int sx, int sy, int ex, int ey)
{
	if (!m_Astar)return{};
	//printf(" GameMap:sx %d sy %d ex %d ey %d\n", sx, sy, ex, ey);
	if (sx < 0 || sy < 0 || ex < 0 || ey < 0) return{};
	if (sx >= m_CellWidth || ex >= m_CellWidth || sy >= m_CellHeight || ey >= m_CellHeight) return{};
	if (sx == ex && sy == ey) return{};
	if (m_Cell[sx][sy] > 0 || m_Cell[ex][ey] > 0) return{};

	if (m_Astar->PathFinding(sx, sy, ex, ey))
	{
		auto path = m_Astar->GetMoveList();
		std::vector<Pos> moveList(path.begin(), path.end());

		// Smooth move List
		/*
		算法：
		1.第一个节点为当前节点，查看下一个节点
		2.如果可以无障碍直接到达，则下一个节点为第一个节点的后继
		3.如果不可到达，当前节点为后继节点
		*/
		int currentNode = 0;
		int nextNode = 1;
		std::list<Pos> smoothMoveList;

		if (moveList.size()>0)
		{
			smoothMoveList.push_back(moveList[currentNode]);
			while (nextNode != moveList.size())
			{
				int lastNextNode = nextNode;

				while (nextNode != moveList.size() && CanArriveDirect(moveList[currentNode], moveList[nextNode]))
				{
					lastNextNode = nextNode;
					nextNode++;
				}
				currentNode = lastNextNode;
				smoothMoveList.push_back(moveList[currentNode]);
			}
		}
		return smoothMoveList;
	}
	else {
		return{};
	}
}

GameMap::~GameMap()
{
	if (m_Astar != nullptr)
	{
		delete m_Astar;
		m_Astar = nullptr;
	}

	if (m_XyqMap != nullptr) {
		delete m_XyqMap;
		m_XyqMap = nullptr;
	}
}

void GameMap::Update()
{
}
int GameMap::GetMapOffsetX()
{
	return m_MapOffsetX;
}
int GameMap::GetMapOffsetY()
{
	return m_MapOffsetY;
}


