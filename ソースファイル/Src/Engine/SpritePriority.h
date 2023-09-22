//@file SpritePriority.h

#ifndef SPRITEPRIORITY_H_INCLUDED
#define SPRITEPRIORITY_H_INCLUDED

//表示優先順位

enum SpritePriority
{//優先順位（高）
	prioPlayer,			//自機
	prioEnemyBullet,	//敵の弾
	prioPlayerBullet,	//自機の弾
	prioExplosion,		//爆発エフェクト
	prioEnemy,			//敵
	prioItem,			//アイテム
};//優先順位（低）




#endif  SPRITEPRIORITY_H_INCLUDED