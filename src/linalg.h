/*
* @file linalg.h
* @author J. Keson
*/
#pragma once
#include <math.h>
#include <string.h>

#define FOREACH(v, n) for (unsigned int v = 0; v < n; v++)


template <int n>
class vector {
	float v[n];
public:

	constexpr float operator[](int index) 
	{ return v[index]; }
	
	void zero()
	{FOREACH(i, n) v[i] = 0;}
	constexpr void clear() { zero(); }

	vector& operator=(const vector& vec) {
		FOREACH(i, n) v[i] = vec[i];
	}
	inline vector(const vector& vec) {
		FOREACH(i, n) v[i] = vec[i];
	}
	float dot(const vector& a, const vector& b) {
		float scalar = 0;
		FOREACH(i, n) scalar += a[i] * b[i];
		return scalar;
	}


	/*
	vector& operator+(const vector& a, const vector& b) {
		vector r;
		FOREACH(i = 0; n) r[i] = a[i] + b[i];
	}
	vector& operator-(const vector& a, const vector& b) {
		vector r;
		FOREACH(i = 0; n) r[i] = a[i] - b[i];
	}
	vector& operator +=(const vector& a, ) {
		FOREACH(i = 0, n) v[i] += a[i];
	}
	vector& operator +=(const vector& a, ) {
		FOREACH(i = 0, n) v[i] -= a[i];
	}
	
	vector& operator*(const vector& vec, const float scalar) {
		vector r;
		FOREACH(i = 0, n) r[i] = vec[i] * scalar;
	}
	vector& operator*=(const float scalar) {
		FOREACH(i = 0, n) v[i] *= scalar;
	}
	*/



};

using vec2 = vector<2>;
using vec3 = vector<3>;
using vec4 = vector<4>;


/*Order as follows:
	X, Y, Z, W
*/
typedef vec4 quat;


/*Transform*/
typedef struct {
	quat q; //Orientation quarternion
	vec3 p;	//Position vector
} trans;

