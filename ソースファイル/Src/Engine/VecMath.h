//@file VecMath.h

#ifndef VECMATH_H_INCLUDED
#define VECMATH_H_INCLUDED

#include <cmath>


namespace VecMath
{
	//先行宣言
	struct vec3;
	struct vec4;
	struct mat3;
	struct mat4;



	inline constexpr float pi = 3.14159265f;	//円周率
	inline constexpr float invPi = 1 / pi;		//円周率の逆数

	//度数法から弧度法へ変換
	inline constexpr float radians(float d) { return d / 180 * pi; }

	//弧度法から度数法へ変換
	inline constexpr float degrees(float r) { return r / pi * 180; }

	//2Dベクトル
	struct vec2
	{
		float x, y;

		//コンストラクタ
		vec2() = default;
		explicit vec2(float s) :x(s), y(s) {}
		vec2(float x, float y) :x(x), y(y) {}
		explicit vec2(const vec3& v);

		//加算	a+=bのように書ける
		vec2& operator+=(const vec2& other)
		{
			x += other.x;
			y += other.y;

			return *this;
		}
		vec2& operator+=(float other) { return *this += vec2(other); }
		friend vec2 operator+(vec2 a, const vec2& b) { return a += b; }
		friend vec2 operator+(vec2 a, float b) { return a += b; }
		friend vec2 operator+(float a, const vec2& b) { return vec2(a) += b; }

		//減算	a-=bのように書ける
		vec2& operator-=(const vec2& other)
		{
			x -= other.x;
			y -= other.y;

			return *this;
		}

		vec2& operator-=(float other) { return *this -= vec2(other); }
		friend vec2 operator-(vec2 a, const vec2& b) { return a -= b; }
		friend vec2 operator-(vec2 a, float b) { return a -= b; }
		friend vec2 operator-(float a, const vec2& b) { return vec2(a) -= b; }


		//乗算	a*=bのように書ける
		vec2& operator*=(const vec2& other)
		{
			x *= other.x;
			y *= other.y;

			return *this;
		}

		vec2& operator*=(float other) { return *this *= vec2(other); }
		friend vec2 operator*(vec2 a, const vec2& b) { return a *= b; }
		friend vec2 operator*(vec2 a, float b) { return a *= b; }
		friend vec2 operator*(float a, const vec2& b) { return vec2(a) *= b; }

		//徐算	a/=bのように書ける
		vec2& operator/=(const vec2& other)
		{
			x /= other.x;
			y /= other.y;

			return *this;
		}

		vec2& operator/=(float other) { return *this /= vec2(other); }
		friend vec2 operator/(vec2 a, const vec2& b) { return a /= b; }
		friend vec2 operator/(vec2 a, float b) { return a /= b; }
		friend vec2 operator/(float a, const vec2& b) { return vec2(a) /= b; }

		//単項マイナス演算子
		vec2 operator-() const { return vec2(-x, -y); }

		//添字演算子
		float& operator[](size_t n) { return *(&x + n); }
		const float& operator[](size_t n) const { return *(&x + n); }
	};





	//3Dベクトル
	struct vec3
	{
		float x, y, z;

		//コンストラクタ
		vec3() = default;
		explicit vec3(float s) :x(s), y(s), z(s) {}
		vec3(float x, float y, float z) :x(x), y(y), z(z) {}
		vec3(const vec2& xy, float z) :x(xy.x), y(xy.y), z(z) {}
		explicit vec3(const vec4& v);

		//加算	a+=bのように書ける
		vec3& operator+=(const vec3& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;

			return *this;
		}

		vec3& operator+=(float other) { return *this += vec3(other); }
		friend vec3 operator+(vec3 a, const vec3& b) { return a += b; }
		friend vec3 operator+(vec3 a, float b) { return a += b; }
		friend vec3 operator+(float a, const vec3& b) { return vec3(a) += b; }

		//減算	a-=bのように書ける
		vec3& operator-=(const vec3& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;

			return *this;
		}

		vec3& operator-=(float other) { return *this -= vec3(other); }
		friend vec3 operator-(vec3 a, const vec3& b) { return a -= b; }
		friend vec3 operator-(vec3 a, float b) { return a -= b; }
		friend vec3 operator-(float a, const vec3& b) { return vec3(a) -= b; }


		//乗算	a*=bのように書ける
		vec3& operator*=(const vec3& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;

			return *this;
		}

		vec3& operator*=(float other) { return *this *= vec3(other); }
		friend vec3 operator*(vec3 a, const vec3& b) { return a *= b; }
		friend vec3 operator*(vec3 a, float b) { return a *= b; }
		friend vec3 operator*(float a, const vec3& b) { return vec3(a) *= b; }




		//徐算	a/=bのように書ける
		vec3& operator/=(const vec3& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;

			return *this;
		}

		vec3& operator/=(float other) { return *this /= vec3(other); }
		friend vec3 operator/(vec3 a, const vec3& b) { return a /= b; }
		friend vec3 operator/(vec3 a, float b) { return a /= b; }
		friend vec3 operator/(float a, const vec3& b) { return vec3(a) /= b; }

		//単項マイナス演算子
		vec3 operator-() const { return vec3(-x, -y, -z); }

		//添字演算子
		float& operator[](size_t n) { return *(&x + n); }
		const float& operator[](size_t n) const { return *(&x + n); }


	};

	//内積
	inline float dot(const vec3& a, const vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	//長さ
	inline float  length(const vec3& v)
	{
		return sqrt(dot(v, v));
	}

	//正規化
	inline vec3 normalize(const vec3& v)
	{
		const float d = 1.0f / length(v);
		return v * d;
	}

	//外積
	inline vec3 cross(const vec3& a, const vec3& b)
	{
		return vec3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	//vec3からvec2への変換コンストラクタ
	inline vec2::vec2(const vec3& v) :
		x(v.x), y(v.y)
	{
	}

	//4Dのベクトル
	struct vec4
	{
		float x, y, z, w;

		//コンストラクタ
		vec4() = default;
		explicit vec4(float s) :x(s), y(s), z(s), w(s) {}
		vec4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w) {}
		vec4(const vec2& xy, float z, float w) :x(xy.x), y(xy.y), z(z), w(w) {}
		vec4(const vec3& xyz, float w) :x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}

		//加算	a+*=bのように書ける
		vec4& operator+=(const vec4& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;

			return *this;
		}

		vec4& operator+=(float other) { return *this += vec4(other); }
		friend vec4 operator+(vec4 a, const vec4& b) { return a += b; }
		friend vec4 operator+(vec4 a, float b) { return a += b; }
		friend vec4 operator+(float a, const vec4& b) { return vec4(a) += b; }

		//減算	a-=bのように書ける
		vec4& operator-=(const vec4& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;

			return *this;
		}

		vec4& operator-=(float other) { return *this -= vec4(other); }
		friend vec4 operator-(vec4 a, const vec4& b) { return a -= b; }
		friend vec4 operator-(vec4 a, float b) { return a -= b; }
		friend vec4 operator-(float a, const vec4& b) { return vec4(a) -= b; }

		//乗算	a*=bのように書ける
		vec4& operator*=(const vec4& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;

			return *this;
		}

		vec4& operator*=(float other) { return *this *= vec4(other); }
		friend vec4 operator*(vec4 a, const vec4& b) { return a *= b; }
		friend vec4 operator*(vec4 a, float b) { return a *= b; }
		friend vec4 operator*(float a, const vec4& b) { return vec4(a) *= b; }

		//徐算	a/=bのように書ける
		vec4& operator/=(const vec4& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			w /= other.w;

			return *this;
		}

		vec4& operator/=(float other) { return *this /= vec4(other); }
		friend vec4 operator/(vec4 a, const vec4& b) { return a /= b; }
		friend vec4 operator/(vec4 a, float b) { return a /= b; }
		friend vec4 operator/(float a, const vec4& b) { return vec4(a) /= b; }

		//単項マイナス演算子
		vec4 operator-() const { return vec4(-x, -y, -x, -w); }
		
		//添字演算子
		float& operator[](size_t n) { return *(&x + n); }
		const float& operator[](size_t n) const { return *(&x + n); }

	};

	//vec4からvec3への変換コンストラクタ
	inline vec3::vec3(const vec4& v) :
		x(v.x), y(v.y), z(v.z)
	{
	}

	//3x3行列
	struct mat3
	{
		vec3 data[3];

		//コンストラクタ
		mat3() = default;
		mat3(const vec3& v0, const vec3& v1, const vec3& v2)
		{
			data[0] = v0;
			data[1] = v1;
			data[2] = v2;
		}

		explicit mat3(float s)
		{
			data[0] = vec3(s, 0, 0);
			data[1] = vec3(0, s, 0);
			data[2] = vec3(0, 0, s);
		}

		//mat4->mat3に変換コンストラクタ
		explicit mat3(const mat4& m);

		//添え字演算子
		vec3& operator[](size_t n) { return data[n]; }
		const vec3& operator[](size_t n)const { return data[n]; }

		friend vec3 operator*(mat3 a, const vec3& b)
		{
			return a[0] * b.x + a[1] * b.y + a[2] * b.z;
		}

		
	};

	mat3 transpose(const mat3& m);	//転置行列(もとの行列の行と烈を入れ替える)
	mat3 inverse(const mat3& m);	//逆行列

	//4x4行列
	struct mat4
	{
		vec4 data[4];

		//コンストラクタ
		mat4() = default;
		mat4(const vec4& v0, const vec4& v1, const vec4& v2,const vec4 v3)
		{
			data[0] = v0;
			data[1] = v1;
			data[2] = v2;
			data[3] = v3;
		}

		//インスタンス化を制限
		explicit mat4(float s)
		{
			data[0] = vec4(s, 0, 0, 0);
			data[1] = vec4(0, s, 0, 0);
			data[2] = vec4(0, 0, s, 0);
			data[3] = vec4(0, 0, 0, s);
		}

		//添え字演算子
		vec4& operator[](size_t n) { return data[n]; }
		const vec4& operator[](size_t n)const { return data[n]; }

		//行列とベクトル乗算
		friend vec4 operator*(const mat4& m, const vec4& v)
		{
			return m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w;
		}

		//行列と行列の乗算
		mat4& operator*=(const mat4& other);
		friend mat4 operator*(const mat4& m0, const mat4& m1)
		{
			return mat4(m0) *= m1;
		}

		static mat4 Translate(const vec3& v);	//平行移動行列を作成
		static mat4 Scale(const vec3& v);		//拡大縮小を作成
		static mat4 RotateX(float angle);		//X軸回転行列を作成
		static mat4 RotateY(float angle);		//Y軸回転行列を作成
		static mat4 RotateZ(float angle);		//Z軸回転行列を作成

		//ビュー行列を計算
		static mat4 LookAt(const vec3& eye, const vec3& target, const vec3& up);
	
		//垂直投影行列
		static mat4 Orthogonal(float left, float right, float bottom, float top,
			float zNear, float zFar);

		//透視投影行列を求める関数を定義する
		static mat4 Perspective(float fovy, float aspectRatio, float zNear, float zFar);
	};

	//mat4からmat3への変換コンストラクタ
	inline mat3::mat3(const mat4& m)
	{
		data[0] = vec3(m[0]);
		data[1] = vec3(m[1]);
		data[2] = vec3(m[2]);
	}

	mat4 transpose(const mat4& m);	//転置行列(もとの行列の行と烈を入れ替える)
	mat4 inverse(const mat4& m);	//逆行列

}









#endif //VECMATH_H_INCLUDED