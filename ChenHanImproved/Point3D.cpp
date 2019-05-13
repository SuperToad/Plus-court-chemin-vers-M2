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
// Point3D.cpp: implementation of the CPoint3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Point3D.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoint3D::CPoint3D()
{
}

CPoint3D::CPoint3D(double x, double y, double z) : x(x), y(y), z(z)
{
}

CPoint3D operator +(const CPoint3D& pt1, const CPoint3D& pt2)
{
	return CPoint3D(pt1.x + pt2.x, pt1.y + pt2.y, pt1.z + pt2.z);
}

CPoint3D operator -(const CPoint3D& pt1, const CPoint3D& pt2)
{
	return CPoint3D(pt1.x - pt2.x, pt1.y - pt2.y, pt1.z - pt2.z);
}

CPoint3D operator *(const CPoint3D& pt, double times)
{
	return CPoint3D(pt.x * times, pt.y * times, pt.z * times);
}
	
CPoint3D operator *(double times, const CPoint3D& pt)
{
	return CPoint3D(pt.x * times, pt.y * times, pt.z * times);
}

CPoint3D operator*(const CPoint3D& pt1, const CPoint3D& pt2)
{
	return CPoint3D(pt1.y * pt2.z - pt1.z * pt2.y,
		pt1.z * pt2.x - pt1.x * pt2.z,
		pt1.x * pt2.y - pt1.y * pt2.x);
}

CPoint3D VectorCross(const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3)
{
	return (pt2 - pt1) * (pt3 - pt2);
}

double operator ^(const CPoint3D& pt1, const CPoint3D& pt2)
{
	return pt1.x * pt2.x + pt1.y * pt2.y + pt1.z * pt2.z;
}

double GetTriangleArea(const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3)
{
	CPoint3D crossProduct = (pt2 - pt1) * (pt3 - pt2);
	return 0.5 * crossProduct.Len();
}

double AngleBetween(const CPoint3D& pt1, const CPoint3D& pt2)
{
	double cosAngle = (pt1 ^ pt2) / pt1.Len() / pt2.Len();
	if (cosAngle >= 1)
	{
		cosAngle = 1;
	}
	else if (cosAngle <= -1)
	{
		cosAngle = -1;
	}
	return acos(cosAngle);
}

double AngleBetween(const CPoint3D& pt1, const CPoint3D& pt2, const CPoint3D& pt3)
{
	CPoint3D u = pt2 - pt1;
	CPoint3D v = pt3 - pt2;
	double cosAngle = (u ^ v) / u.Len() / v.Len();
	if (cosAngle >= 1)
	{
		cosAngle = 1;
	}
	else if (cosAngle <= -1)
	{
		cosAngle = -1;
	}
	return acos(cosAngle);
}

float VectorDot(const float* u, const float* v)
{
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void VectorCross(const float* u, const float* v, float* n)
{
	n[0] = u[1] * v[2] - u[2] * v[1];
	n[1] = u[2] * v[0] - u[0] * v[2];
	n[2] = u[0] * v[1] - u[1] * v[0];
}

float AngleBetween(const float* u, const float* v)
{
	float lenU = sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
	float lenV = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	float dot = VectorDot(u, v) / lenU / lenV;
	if (dot < -1)
		dot = -1.0;
	if (dot > 1)
		dot = 1.0;	
	return acos(dot);
}