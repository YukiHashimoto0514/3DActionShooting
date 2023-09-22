//@file VecMAth.cpp

#include "VecMath.h"

namespace VecMath
{

	//転置行列を求める
	mat3 transpose(const mat3& m)
	{
		mat3 t;

		t[0][0] = m[0][0];
		t[0][1] = m[1][0];
		t[0][2] = m[2][0];

		t[1][0] = m[0][1];
		t[1][1] = m[1][1];
		t[1][2] = m[2][1];

		t[2][0] = m[0][2];
		t[2][1] = m[1][2];
		t[2][2] = m[2][2];

		return t;
	}

	//逆行列を求める
	mat3 inverse(const mat3& m)
	{
		//余因子行列を計算
		mat3 adjugate;
		adjugate[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		adjugate[0][1] = -m[0][1] * m[2][2] + m[0][2] * m[2][1];
		adjugate[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];

		adjugate[1][0] = -m[1][0] * m[2][2] + m[1][2] * m[2][0];
		adjugate[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
		adjugate[1][2] = -m[0][0] * m[1][2] + m[0][2] * m[1][0];

		adjugate[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
		adjugate[2][1] = -m[0][0] * m[2][1] + m[0][1] * m[2][0];
		adjugate[2][2] = m[0][0] * m[1][2] - m[0][1] * m[1][0];

		//行列式を計算
		const float det =
			m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] +
			m[0][1] * m[1][2] * m[2][0] - m[0][1] * m[1][0] * m[2][2] +
			m[0][2] * m[1][0] * m[2][1] - m[0][2] * m[1][1] * m[2][0];

		//行列式の逆数を仕掛ける
		const float invDet = 1.0f / det;

		adjugate[0] *= invDet;
		adjugate[1] *= invDet;
		adjugate[2] *= invDet;

		return adjugate;
	}

	//転置行列を求める
	mat4 transpose(const mat4& m)
	{
		mat4 t;

		t[0][0] = m[0][0];
		t[0][1] = m[1][0];
		t[0][2] = m[2][0];
		t[0][3] = m[3][0];

		t[1][0] = m[0][1];
		t[1][1] = m[1][1];
		t[1][2] = m[2][1];
		t[1][3] = m[3][1];

		t[2][0] = m[0][2];
		t[2][1] = m[1][2];
		t[2][2] = m[2][2];
		t[2][3] = m[3][2];

		t[3][0] = m[0][3];
		t[3][1] = m[1][3];
		t[3][2] = m[2][3];
		t[3][3] = m[3][3];

		return t;
	}

	//逆行列を求める
	mat4 inverse(const mat4& m)
	{
		//余因子行列を計算
		mat4 adjugate;
		adjugate[0][0] = m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2]
			- m[1][3] * m[2][2] * m[3][1] - m[1][2] * m[2][1] * m[3][3] - m[1][1] * m[2][3] * m[3][2];

		adjugate[0][1] = -m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2]
			+ m[0][3] * m[2][2] * m[3][1] + m[0][2] * m[2][1] * m[3][3] + m[0][1] * m[2][3] * m[3][2];

		adjugate[0][2] = m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2]
			- m[0][3] * m[1][2] * m[3][1] - m[0][2] * m[1][1] * m[3][3] - m[0][1] * m[1][3] * m[3][2];

		adjugate[0][3] = -m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2]
			+ m[0][3] * m[1][2] * m[2][1] + m[0][2] * m[1][1] * m[2][3] + m[0][1] * m[1][3] * m[2][2];


		adjugate[1][0] = -m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2]
			+ m[1][3] * m[2][2] * m[3][0] + m[1][2] * m[2][0] * m[3][3] + m[1][0] * m[2][3] * m[3][2];

		adjugate[1][1] = m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2]
			- m[0][3] * m[2][2] * m[3][0] - m[0][2] * m[2][0] * m[3][3] - m[0][0] * m[2][3] * m[3][2];

		adjugate[1][2] = -m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2]
			+ m[0][3] * m[1][2] * m[3][0] + m[0][2] * m[1][0] * m[3][3] + m[0][0] * m[1][3] * m[3][2];

		adjugate[1][3] = m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2]
			- m[0][3] * m[1][2] * m[2][0] - m[0][2] * m[1][0] * m[2][3] - m[0][0] * m[1][3] * m[2][2];


		adjugate[2][0] = m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1]
			- m[1][3] * m[2][1] * m[3][0] - m[1][1] * m[2][0] * m[3][3] - m[1][0] * m[2][3] * m[3][1];

		adjugate[2][1] = -m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1]
			+ m[0][3] * m[2][1] * m[3][0] + m[0][1] * m[2][0] * m[3][3] + m[0][0] * m[2][3] * m[3][1];

		adjugate[2][2] = m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1]
			- m[0][3] * m[1][1] * m[3][0] - m[0][1] * m[1][0] * m[3][3] - m[0][0] * m[1][3] * m[3][1];

		adjugate[2][3] = -m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1]
			+ m[0][3] * m[1][1] * m[2][0] + m[0][1] * m[1][0] * m[2][3] + m[0][0] * m[1][3] * m[2][1];


		adjugate[3][0] = -m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][3] * m[2][0] * m[3][1]
			+ m[1][2] * m[2][1] * m[3][0] + m[1][1] * m[2][0] * m[3][2] + m[1][0] * m[2][2] * m[3][1];

		adjugate[3][1] = m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1]
			- m[0][2] * m[2][1] * m[3][0] - m[0][1] * m[2][0] * m[3][2] - m[0][0] * m[2][2] * m[3][1];

		adjugate[3][2] = -m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1]
			+ m[0][2] * m[1][1] * m[3][0] + m[0][1] * m[1][0] * m[3][2] + m[0][0] * m[1][2] * m[3][1];

		adjugate[3][3] = m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1]
			- m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];



		//行列式を計算
		const float det =
			m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][0] * m[1][3] * m[2][1] * m[3][2] -
			m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][0] * m[1][1] * m[2][3] * m[3][2] -
			m[0][1] * m[1][0] * m[2][2] * m[3][3] - m[0][2] * m[1][0] * m[2][3] * m[3][1] - m[0][3] * m[1][0] * m[2][1] * m[3][2] +
			m[0][3] * m[1][0] * m[2][2] * m[3][1] + m[0][2] * m[1][0] * m[2][1] * m[3][3] + m[0][1] * m[1][0] * m[2][3] * m[3][2] +
			m[0][1] * m[1][2] * m[2][0] * m[3][3] + m[0][2] * m[1][3] * m[2][0] * m[3][1] + m[0][3] * m[1][1] * m[2][0] * m[3][2] -
			m[0][3] * m[1][2] * m[2][0] * m[3][3] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -
			m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0] - m[0][3] * m[1][1] * m[2][2] * m[3][0] +
			m[0][3] * m[1][2] * m[2][1] * m[3][0] + m[0][2] * m[1][1] * m[2][3] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0];

		//行列式の逆数を仕掛ける
		const float invDet = 1.0f / det;

		adjugate[0] *= invDet;
		adjugate[1] *= invDet;
		adjugate[2] *= invDet;
		adjugate[3] *= invDet;

		return adjugate;
	}


	//行列同士の乗算
	mat4& mat4::operator*=(const mat4& other)
	{
		const mat4 m = *this;
		data[0] = m * other[0];
		data[1] = m * other[1];
		data[2] = m * other[2];
		data[3] = m * other[3];

		return *this;
	}

	//平行移動行列を作成する
	mat4 mat4::Translate(const vec3& v)
	{
		mat4 m(1);
		m[3][0] = v.x;
		m[3][1] = v.y;
		m[3][2] = v.z;

		return m;
	}

	//拡大縮小を作成する
	mat4 mat4::Scale(const vec3& v)
	{
		mat4 m(1);
		m[0][0] = v.x;
		m[1][1] = v.y;
		m[2][2] = v.z;

		return m;
	}

	//X軸回転行列を作成する
	mat4 mat4::RotateX(float angle)
	{
		const float c = cos(angle);
		const float s = sin(angle);

		mat4 m(1);

		//y'= y * c - z * s
		//z'= y * s + z * c
		m[1][1] = c;
		m[2][1] = -s;
		m[1][2] = s;
		m[2][2] = c;

		return m;
	}

	//Y軸回転行列を作成する
	mat4 mat4::RotateY(float angle)
	{
		const float c = cos(angle);
		const float s = sin(angle);

		mat4 m(1);

		//x'= x * c + z * s
		//z'= x * -s + z * c
		m[0][0] = c;
		m[2][0] = s;
		m[0][2] = -s;
		m[2][2] = c;

		return m;

	}

	//Z軸回転行列を作成する
	mat4 mat4::RotateZ(float angle)
	{
		const float c = cos(angle);
		const float s = sin(angle);

		mat4 m(1);

		//x'= z * c + z * s
		//y'= z * -s + z * c
		m[0][0] = c;
		m[1][0] = -s;
		m[0][1] = s;
		m[1][1] = c;

		return m;

	}

	//ビュー行列を作成する

	//@param eye    視点の座標
	//@param target 注視店の座標
	//@param up     視点の上方向を指すベクトル

	//return eye,target,up から作成したビュー行列

	mat4 mat4::LookAt(const vec3& eye, const vec3& target, const vec3& up)
	{
		//ワールド座標系における注視店座標系のXYZの向きを計算
		const vec3 axisZ = normalize(eye - target);
		const vec3 axisX = normalize(cross(up, axisZ));
		const vec3 axisY = cross(axisZ, axisX);

		//座標を各軸に射影するように行列の値を設定
		mat4 m(1);
		m[0][0] = axisX.x;
		m[1][0] = axisX.y;
		m[2][0] = axisX.z;

		m[0][1] = axisY.x;
		m[1][1] = axisY.y;
		m[2][1] = axisY.z;

		m[0][2] = axisZ.x;
		m[1][2] = axisZ.y;
		m[2][2] = axisZ.z;

		//平行移動の値を求めるため、視点座標を各軸に射影する
		m[3][0] = -dot(axisX, eye);
		m[3][1] = -dot(axisY, eye);
		m[3][2] = -dot(axisZ, eye);

		return m;
	}


	//平行投影行列を作成する

	//@param left    描画範囲の左端までの距離
	//@param right   描画範囲の右端までの距離
	//@param bottom  描画範囲の下端までの距離
	//@param top     描画範囲の上端までの距離
	//@param zNear   描画範囲に含まれる最小ｚ座標
	//@param zFar    描画範囲に含まれる最大ｚ座標

//return 垂直投影行列

	mat4 mat4::Orthogonal(float left, float right, float bottom, float top,
		float zNear, float zFar)
	{
		//拡大率を設定
		mat4 m(1);
		m[0][0] = 2 / (right - left);
		m[1][1] = 2 / (top - bottom);
		m[2][2] = -2 / (zFar - zNear);//z反転

		//平行移動を設定
		m[3][0] = -(right + left) / (right - left);
		m[3][1] = -(top + bottom) / (top - bottom);
		m[3][2] = -(zFar + zNear) / (zFar - zNear);

		return m;

	}

	//透視投影行列を作成する

	//@param fovy          垂直視野角(ラジアン)
	//@param aspectRatio   垂直視野角に対する水平視野角の比率
	//@param zNear         描画範囲に含まれる最小ｚ座標
	//@param zFar          描画範囲に含まれる最大ｚ座標

	mat4 mat4::Perspective(float fovy, float aspectRatio,float zNear, float zFar)
	{
		const float tanHalfFovy = tan(fovy / 2);

		mat4 Result(0);
		Result[0][0] = 1 / (aspectRatio * tanHalfFovy);
		Result[1][1] = 1 / (tanHalfFovy);
		Result[2][2] = -(zFar + zNear) / (zFar - zNear);
		Result[2][3] = -1;
		Result[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

		return Result;

	}


}