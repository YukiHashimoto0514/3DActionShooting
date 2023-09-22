//@file TileMap.h

#ifndef TILEMAP_H_INCLUDED
#define TILEMAP_H_INCLUDED
#include <vector>

//タイルの配置データ
struct TileMap{
	size_t sizeX = 0;//横のタイル数
	size_t sizeY = 0;//縦のタイル数

	std::vector<int> data;//配置データ
};





#endif  TILEMAP_H_INCLUDED
