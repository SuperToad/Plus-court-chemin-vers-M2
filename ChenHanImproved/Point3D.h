// Copyright (c) 2005-2012 Shi-Qing Xin (xinshiqing@163.com) and Guo-Jin Wang (wanggj@zju.edu.cn).
// NOTE: this is an old version. For the lastest version, please email us.
// The code is free for research purpose, but requires a token charge for commercial purpose. 
// Users are forbidden to reproduce, republish, redistribute, or resell the code without our permission.
//
// In this code, we implemented chen and han's algorithm [1990].
// Furthermore, we gave two techniques to improve the CH algorithm.
// If you have any ideas about improving the code, we are very grateful.
// My personal website: http://sites.google.com/site/xinshiqing/Home
// 
// We are debted to Prof. Han, who gave us many helpful ideas.
// We must thank Surazhsky and Kirsanov for their knowledge share.
//
// Point3D.h: interface for the CPoint3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT3D_H__2655648F_9CC2_42AA_9582_9108241B1220__INCLUDED_)
#define AFX_POINT3D_H__2655648F_9CC2_42AA_9582_9108241B1220__INCLUDED_
#include <math.h>

struct CPoint3D  
{
public:
	double x, y, z;
	CPoint3D();
	CPoint3D(double x, double y, double z);	
	inline CPoint3D& operator +=(const CPoint3D& pt);
	inline CPoint3D& operator -=(const CPoint3D& pt);
	inline CPoint3D& operator *=(double times);
	inline CPoint3D& operator /=(double times);
	inline CPoint3D operator /(double times) const;
	inline double Len() const;
	inline void Normalize();
};

CPoint3D& CPoint3D::operator +=(const CPoint3D& pt)
{
	x += pt.x;
	y += pt.y;
	z += pt.z;
	return *this;
}

CPoint3D& CPoint3D::operator -=(const CPoint3D& pt)
{
	x -= pt.x;
	y -= pt.y;
	z -= pt.z;
	return *this;
}

CPoint3D& CPoint3D::operator *=(double times)
{
	x *= times;
	y *= times;
	z *= times;
	return *this;
}

CPoint3D& CPoint3D::operator /=(double times)
{
	x /= times;
	y /= times;
	z /= times;
	return *this;
}

CPoint3D CPoint3D::operator /(double times) const
{
	return CPoint3D(x / times, y / times, z / times);
}

double CPoint3D::Len() const
{
	return sqrt(x * x + y * y + z * z);
}

void CPoint3D::Normalize()
{
	double len = Len();
	x /= len;
	y /= len;
	z /= len;
}

CPoint3D operator +(const CPoint3D& pt1, const CPoint3D& pt2);
CPoint3D operator -(const CPoint3D& pt1, const CPoint3D& pt2);
CPoint3D operator *(const CPoint3D& pt, double times);
CPoint3D operator *(double times, const CPoint3D& pt);
CPoint3D operator *(const CPoint3D& pt1, const CPoint3D& pt2);
CPoint3D VectorCross(const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3);
double operator ^(const CPoint3D& pt1, const CPoint3D& pt2);
double GetTriangleArea(const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3);
double AngleBetween(const CPoint3D& pt1, const CPoint3D& pt2);
double AngleBetween(const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3);
void VectorCross(const float* u, const float* v, float * n);
float VectorDot(const float* u, const float* v);
float AngleBetween(const float* u, const float* v);
#endif // !defined(AFX_POINT3D_H__2655648F_9CC2_42AA_9582_9108241B1220__INCLUDED_)
