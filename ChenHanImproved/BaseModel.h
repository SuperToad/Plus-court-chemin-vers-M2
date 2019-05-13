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
// BaseModel.h: interface for the CBaseModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJMODEL_H__138204A2_0445_4BA8_B76D_6E5374975C97__INCLUDED_)
#define AFX_OBJMODEL_H__138204A2_0445_4BA8_B76D_6E5374975C97__INCLUDED_

#include "Point3D.h"
#include <fstream>
using namespace std;
class CBaseModel  
{
public:	
	CBaseModel(char * filename);
public:
	struct CFace
	{
		int verts[3];
		CFace(){}
		CFace(int x, int y, int z)
		{
			verts[0] = x;
			verts[1] = y;
			verts[2] = z;
		}
		int& operator[](int index)
		{
			return verts[index];
		}
		int operator[](int index) const
		{
			return verts[index];
		} 
	};
	
protected:	
	void ReadFile(char * filename);
	void ReadObjFile(char * filename);
	void ReadPlyFile(char * filename);
	void ReadAsciiPly(char * filename);
	void ReadBinaryPly(char * filename, const char* format);	
	void ReadUntilNextLine(ifstream& in) const;
	void ReadIntoWord(ifstream &in, string& word) const;
	static void SwapOrder(char *buf, int sz);	
	void AdjustScaleAndComputeNormalsToVerts();
public:
	inline int GetNumOfVerts() const;
	inline int GetNumOfFaces() const;
	void LoadModel();
	string GetFileName() const
	{
		return string(m_filename);
	}
	inline const CPoint3D& Vert(int vertIndex) const;
	inline const CPoint3D& Normal(int vertIndex) const;
	inline const CFace& Face(int faceIndex) const;
	inline bool HasBeenLoad() const;
protected:
	char m_filename[256];
	vector<CPoint3D> m_Verts;
	vector<CFace> m_Faces;
	vector<CPoint3D> m_NormalsToVerts;
	bool m_fBeLoaded;
};

int CBaseModel::GetNumOfVerts() const
{
	return (int)m_Verts.size();
}

int CBaseModel::GetNumOfFaces() const
{
	return (int)m_Faces.size();
}

const CPoint3D& CBaseModel::Vert(int vertIndex) const
{
	return m_Verts[vertIndex];
}

const CPoint3D& CBaseModel::Normal(int vertIndex) const
{
	return m_NormalsToVerts[vertIndex];
}

const CBaseModel::CFace& CBaseModel::Face(int faceIndex) const
{
	return m_Faces[faceIndex];
}

bool CBaseModel::HasBeenLoad() const
{
	return m_fBeLoaded;
}
#endif // !defined(AFX_OBJMODEL_H__138204A2_0445_4BA8_B76D_6E5374975C97__INCLUDED_)
