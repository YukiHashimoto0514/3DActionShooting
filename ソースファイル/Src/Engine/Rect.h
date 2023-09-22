//@file Rect.h

#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED
#include <vector>


//2Dの長方形
struct Rect {
	float min[2];
	float max[2];
};

//2D長方形の配列
using RectList = std::vector<Rect>;

//長方形の座標を移動させる
inline Rect Add(const Rect& r, float x, float y)
{
	return { r.min[0] + x,r.min[1] + y,r.max[0] + x,r.max[1] + y };
}

//2つの長方形が重なっているかしらべる
//@retval true  重なっている
//@retval false 重なっていない

inline bool Overlap(const Rect& a, const Rect& b)
{
	return a.min[0] <= b.max[0] && b.min[0] <= a.max[0] &&
		   a.min[1] <= b.max[1] && b.min[1] <= a.max[1];
}

//2つの長方形配列が重なっているかしらべる
//@retval true  重なっている
//@retval false 重なっていない

inline bool Overlap(const RectList& ra, const RectList& rb)
{
	for (const auto& a : ra)
	{
		for (const auto& b : rb)
		{
			if (Overlap(a, b))
			{
				return true;
			}
		}
	}
	return false;
}

#endif // !RECT_H_INCLUDED
