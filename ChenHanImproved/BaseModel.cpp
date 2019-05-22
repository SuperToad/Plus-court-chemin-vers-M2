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
// ObjModel.cpp: implementation of the CBaseModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseModel.h"
#include <cstring>
#include <cfloat>
#include <stdio.h>
#include <ctype.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBaseModel::CBaseModel(char * filename) 
{
	memcpy(m_filename, filename, sizeof(m_filename));
	m_fBeLoaded = false;
}

void CBaseModel::ReadObjFile(char * filename)
{
	ifstream in(filename, ios_base::in | ios_base::binary);
	if (in.fail())
		throw "Fail to read file!";
	ifstream::pos_type posOfFace;
	map<string, int> uniqueVerts;
	vector<int> vertCorrespondence;
	while (!in.eof())
	{
		string firstWord;
		ReadIntoWord(in, firstWord);
		if (firstWord == "v")
		{
			ostringstream outStr;
			CPoint3D temp;
			in >> temp.x >> temp.y >> temp.z; 
			if (fabs(temp.x) < FLT_EPSILON)
				temp.x = 0;
			if (fabs(temp.y) < FLT_EPSILON)
				temp.y = 0;
			if (fabs(temp.z) < FLT_EPSILON)
				temp.z = 0;
			outStr << setiosflags(ios_base::fixed) << setprecision(5) << temp.x << " " << temp.y << " " << temp.z;
			map<string, int>::iterator pos = uniqueVerts.find(outStr.str());
			if (pos == uniqueVerts.end())
			{
				int oldSize = (int)uniqueVerts.size();
				uniqueVerts[outStr.str()] = oldSize;
				vertCorrespondence.push_back(oldSize);
				m_Verts.push_back(temp);
			}
			else
			{
				vertCorrespondence.push_back(pos->second);
			}
		}
		else if (firstWord == "f")
		{
			in.putback('f');
			posOfFace = in.tellg();
			break;			
		}
		ReadUntilNextLine(in);
	}
    vector<CPoint3D> test(m_Verts);
    m_Verts.swap(test);
	string temp;
	ReadIntoWord(in, temp);
	ReadIntoWord(in, temp);
	in.seekg(posOfFace);
	if (temp.find('/') == -1)
	{
		while (!in.eof())
		{
			string firstWord;
			ReadIntoWord(in, firstWord);
			if (firstWord == "f")
			{
				m_Faces.push_back(CFace());
				CFace& lastFace = m_Faces[m_Faces.size() - 1];			
				in >> lastFace[0] >> lastFace[1] >> lastFace[2];
				for (int j = 0; j < 3; ++j)
				{
					lastFace[j] = vertCorrespondence[lastFace[j] - 1];
				}
			}
			else
				break;
			ReadUntilNextLine(in);			
		}
	}
	else
	{
		while (!in.eof())
		{
			string firstWord;
			ReadIntoWord(in, firstWord);
			if (firstWord == "f")
			{
				m_Faces.push_back(CFace());
				CFace& lastFace = m_Faces[m_Faces.size() - 1];			
				in >> lastFace[0] >> firstWord >> lastFace[1] >> firstWord >> lastFace[2];
				for (int j = 0; j < 3; ++j)
				{
					lastFace[j] = vertCorrespondence[lastFace[j] - 1];
				}
			}
			else
				break;
			ReadUntilNextLine(in);
		}
	}
	in.close();
    vector<CFace> testF(m_Faces);
    m_Faces.swap(testF);
}

void CBaseModel::AdjustScaleAndComputeNormalsToVerts()
{
	if (m_Verts.empty())
		return;
    m_NormalsToVerts.resize(m_Verts.size(), CPoint3D(0, 0, 0));
    /*CPoint3D center(0, 0, 0);
	double sumArea(0);
	CPoint3D sumNormal(0, 0, 0);
	double deta(0);
	for (int i = 0; i < (int)m_Faces.size(); ++i)
	{
		CPoint3D normal = VectorCross(Vert(Face(i)[0]),
			Vert(Face(i)[1]),
			Vert(Face(i)[2]));
		double area = normal.Len();
		CPoint3D gravity3 = Vert(Face(i)[0]) +	Vert(Face(i)[1]) + Vert(Face(i)[2]);
		center += area * gravity3;
		sumArea += area;
		sumNormal += normal;
		deta += gravity3 ^ normal;
		normal.x /= area;
		normal.y /= area;
		normal.z /= area;
		for (int j = 0; j < 3; ++j)
		{
			m_NormalsToVerts[Face(i)[j]] += normal;
		}
	}
	center /= sumArea * 3;
	deta -= 3 * (center ^ sumNormal);
	if (deta > 0)
	{
		for (int i = 0; i < GetNumOfVerts(); ++i)
		{
			if (fabs(m_NormalsToVerts[i].x)
				+ fabs(m_NormalsToVerts[i].y)
				+ fabs(m_NormalsToVerts[i].z) >= FLT_EPSILON)
			{					
				m_NormalsToVerts[i].Normalize();
			}
		}
	}
	else
	{
		for (int i = 0; i < GetNumOfFaces(); ++i)
		{
			int temp = m_Faces[i][0];
			m_Faces[i][0] = m_Faces[i][1];
			m_Faces[i][1] = temp;
		}
		for (int i = 0; i < GetNumOfVerts(); ++i)
		{
			if (fabs(m_NormalsToVerts[i].x)
				+ fabs(m_NormalsToVerts[i].y)
				+ fabs(m_NormalsToVerts[i].z) >= FLT_EPSILON)
			{					
				double len = m_NormalsToVerts[i].Len();
				m_NormalsToVerts[i].x /= -len;
				m_NormalsToVerts[i].y /= -len;
				m_NormalsToVerts[i].z /= -len;
			}
		}
	}

    CPoint3D ptUp(m_Verts[0]);
	CPoint3D ptDown(m_Verts[0]);
	for (int i = 1; i < GetNumOfVerts(); ++i)
	{
		if (m_Verts[i].x > ptUp.x)
			ptUp.x = m_Verts[i].x;
		else if (m_Verts[i].x < ptDown.x)
			ptDown.x = m_Verts[i].x;
		if (m_Verts[i].y > ptUp.y)
			ptUp.y = m_Verts[i].y;
		else if (m_Verts[i].y < ptDown.y)
			ptDown.y = m_Verts[i].y;
		if (m_Verts[i].z > ptUp.z)
			ptUp.z = m_Verts[i].z;
		else if (m_Verts[i].z < ptDown.z)
			ptDown.z = m_Verts[i].z;
	}	

    double maxEdgeLenOfBoundingBox = -1;
	if (ptUp.x - ptDown.x > maxEdgeLenOfBoundingBox)
		maxEdgeLenOfBoundingBox = ptUp.x - ptDown.x;
	if (ptUp.y - ptDown.y > maxEdgeLenOfBoundingBox)
		maxEdgeLenOfBoundingBox = ptUp.y - ptDown.y;
	if (ptUp.z - ptDown.z > maxEdgeLenOfBoundingBox)
		maxEdgeLenOfBoundingBox = ptUp.z - ptDown.z;
    double scale = 2.0 / maxEdgeLenOfBoundingBox;
	for (int i = 0; i < GetNumOfVerts(); ++i)
	{
		m_Verts[i] -= center;
		m_Verts[i] *= scale;
    }	*/
}

void CBaseModel::LoadModel()
{
	if (m_fBeLoaded)
		return;
	ReadFile(m_filename);
	AdjustScaleAndComputeNormalsToVerts();
	m_fBeLoaded = true;
}

void CBaseModel::ReadFile(char * filename)
{
	std::string tmpfilename(filename);
	int nDot = (int)tmpfilename.rfind(L'.');
	if (nDot == -1)
	{
		throw "File name doesn't contain a dot!";
	}

	std::string extension = tmpfilename.substr(nDot + 1);
	for (int i = 0; i < extension.size(); ++i)
	{
		extension[i] = tolower(extension[i]);
	}

	if (extension == "obj")
	{
		ReadObjFile(filename);
	}
	else if (extension == "ply")
	{
		ReadPlyFile(filename);
	}
	else
	{
		throw "This format can't be handled!";
	}

}

void CBaseModel::ReadPlyFile(char * filename)
{
	ifstream in(filename, ios_base::in | ios_base::binary);
	if (in.fail())
	{
		throw "fail to read file";
	}	
	while (!in.eof())
	{
		string firstWord;
		ReadIntoWord(in, firstWord);
		if (firstWord == "format")
			break;
		ReadUntilNextLine(in);
	}
	if (in.eof())
	{
		throw "error model";
	}
	string format;
	ReadIntoWord(in, format);
	in.close();
    //transform(format.begin(), format.end(), format.begin(), tolower);
	if (format.find("ascii") != -1)
	{
		ReadAsciiPly(filename);
	}
	else if (format.find("binary") != -1)
	{
		if (format.find("big") != -1)
			ReadBinaryPly(filename, "big");
		else
			ReadBinaryPly(filename, "little");
	}
	else
	{
		throw "error model format";
	}
}

void CBaseModel::ReadAsciiPly(char * filename)
{
	ifstream in(filename, ios_base::in | ios_base::binary);
	if (in.fail())
	{
		throw "fail to read file";
	}
	int vertNum(0);
	int faceNum(0);
	while (!in.eof())
	{
		string word;
		ReadIntoWord(in, word);
		if (word == "element")
		{
			ReadIntoWord(in, word);
			if (word == "vertex")
			{
				in >> vertNum;
			}
			else if (word == "face")
			{
				in >> faceNum;
			}
		}
		else if (word == "end_header")
		{
			ReadUntilNextLine(in);
			break;
		}
	}
	map<string, int> uniqueVerts;
	m_Verts.reserve(vertNum);
	m_Faces.reserve(faceNum);
	vector<int> vertCorrespondence(vertNum);
	for (int i = 0; i < vertNum; ++i)
	{
		ostringstream outStr;
		CPoint3D temp;
		in >> temp.x >> temp.y >> temp.z; 
		if (fabs(temp.x) < FLT_EPSILON)
			temp.x = 0;
		if (fabs(temp.y) < FLT_EPSILON)
			temp.y = 0;
		if (fabs(temp.z) < FLT_EPSILON)
			temp.z = 0;
		outStr << setiosflags(ios_base::fixed) << setprecision(5) << temp.x << " " << temp.y << " " << temp.z;
		map<string, int>::iterator pos = uniqueVerts.find(outStr.str());
		if (pos == uniqueVerts.end())
		{
			int oldSize = (int)uniqueVerts.size();
			vertCorrespondence[i] = uniqueVerts[outStr.str()] = oldSize;
			m_Verts.push_back(temp);
		}
		else
		{
			vertCorrespondence[i] = pos->second;
		}
		ReadUntilNextLine(in);
	}
    vector<CPoint3D> testV(m_Verts);
    m_Verts.swap(testV);
	for (int i = 0; i < faceNum; ++i)
	{
		int num;
		in >> num;
		vector<int> indices(num);
		for (int j = 0; j < num; ++j)
		{
			int temp;
			in >> temp;
			indices[j] = vertCorrespondence[temp];
		}
		ReadUntilNextLine(in);
		for (int j = 1; j < num - 1; ++j)
		{
			m_Faces.push_back(CFace(indices[0], indices[j], indices[j + 1]));
		}
	}
	in.close();
    vector<CFace> testF(m_Faces);
    m_Faces.swap(testF);
}

void CBaseModel::ReadBinaryPly(char * filename, const char* format)
{
	ifstream in(filename, ios_base::in | ios_base::binary);
	if (in.fail())
	{
		throw "fail to read file";
	}
	map<string, int> types;
	types["char"] = 1;
	types["uchar"] = 1;
	types["short"] = 2;
	types["ushort"] = 2;
	types["int"] = 4;
	types["uint"] = 4;
	types["float"] = 4;
	types["double"] = 8;
	types["int8"] = 1;
	types["int16"] = 2;
	types["int32"] = 4;
	types["uint8"] = 1;
	types["uint16"] = 2;
	types["uint32"] = 4;
	types["float32"] = 4;
	types["float64"] = 8;

	int vertNum(0);
	int faceNum(0);
	while (!in.eof())
	{
		string word;
		ReadIntoWord(in, word);
		if (word == "element")
		{
			ReadIntoWord(in, word);
			if (word == "vertex")
			{
				in >> vertNum;
				break;
			}
		}
	}

	bool isFloatCoordinate(true);
	int sizeOfOneVert(0);
	string word;
	ReadIntoWord(in, word);
	if (!in.eof() && word == "property")
	{
		ReadIntoWord(in, word);
        //transform(word.begin(), word.end(), word.begin(), tolower);
		if (types.find(word) != types.end())
		{
			if (types[word] == 4)
			{
				isFloatCoordinate = true;
			}
			else if (types[word] == 8)
			{
				isFloatCoordinate = false;
			}
			else
			{
				throw "read file error";
			}
			sizeOfOneVert += types[word];
		}
		else
		{
			throw "read file error";
		}
		ReadUntilNextLine(in);
	}
	ReadIntoWord(in, word);
	while (word == "property")
	{
		ReadIntoWord(in, word);
        //transform(word.begin(), word.end(), word.begin(), tolower);
		sizeOfOneVert += types[word];
		ReadUntilNextLine(in);
		ReadIntoWord(in, word);
	}
	while (word != "element" || (ReadIntoWord(in, word), word != "tristrips" && word != "face"))
	{
		ReadUntilNextLine(in);
		ReadIntoWord(in, word);
	}
	
	in >> faceNum;
	if (word == "tristrips")
		faceNum = 0;
	ReadUntilNextLine(in);
	ReadIntoWord(in, word);
	while (word != "property" || (ReadIntoWord(in, word), word != "list"))
	{
		ReadUntilNextLine(in);
		ReadIntoWord(in, word);
	}
	ReadIntoWord(in, word);
    //transform(word.begin(), word.end(), word.begin(), tolower);
	if (types.find(word) == types.end())
	{
		throw "file read error";
	}
	int szOfNumInFace = types[word];
	ReadIntoWord(in, word);
    //transform(word.begin(), word.end(), word.begin(), tolower);
	if (types.find(word) == types.end())
	{
		throw "file read error";
	}
	int szOfVertIndex = types[word];
	do
	{
		ReadIntoWord(in, word);
		ReadUntilNextLine(in);
	} while (word != "end_header");

	map<string, int> uniqueVerts;
	m_Verts.reserve(vertNum);
	m_Faces.reserve(faceNum);
	vector<int> vertCorrespondence(vertNum);

	for (int i = 0; i < vertNum; ++i)
	{
		ostringstream outStr;		
		int szRemaining = sizeOfOneVert;
		CPoint3D pt;
		if (isFloatCoordinate)
		{
			float temp[3];
			szRemaining -= 12;
			for (int j = 0; j < 3; ++j)
			{
				float tmp;
				in.read((char *)&tmp, 4);
				if (format == "big")
				{
					SwapOrder((char*)&tmp, 4);
				}
				if (fabs(tmp) < FLT_EPSILON)
					tmp = 0;
				temp[j] = tmp;
			}
			pt.x = temp[0];
			pt.y = temp[1];
			pt.z = temp[2];
		}
		else
		{
			double temp[3];
			szRemaining -= 24;
			for (int j = 0; j < 3; ++j)
			{
				double tmp;
				in.read((char *)&tmp, 8);
				if (format == "big")
				{
					SwapOrder((char*)&tmp, 8);
				}
				if (fabs(tmp) < FLT_EPSILON)
					tmp = 0;
				temp[j] = tmp;
			}
			pt.x = temp[0];
			pt.y = temp[1];
			pt.z = temp[2];
		}
		outStr << setiosflags(ios_base::fixed) << setprecision(5) << pt.x << " " << pt.y << " " << pt.z;
		map<string, int>::iterator pos = uniqueVerts.find(outStr.str());
		if (pos == uniqueVerts.end())
		{
			int oldSize = (int)uniqueVerts.size();
			vertCorrespondence[i] = uniqueVerts[outStr.str()] = oldSize;
			m_Verts.push_back(pt);
		}
		else
		{
			vertCorrespondence[i] = pos->second;
		}
		if (szRemaining == 0)
			continue;
		char buf[64];
		in.read((char*)&buf, szRemaining);
	}
    vector<CPoint3D> testV(m_Verts);
    m_Verts.swap(testV);
	
	if (faceNum != 0)
	{
		for (int i = 0; i < faceNum; ++i)
		{
			int num(0);
			if (szOfNumInFace == 1)
			{
				char ch;
				in.read(&ch, 1);
				num = ch;
			}
			else if (szOfNumInFace == 2)
			{
				short ch;
				in.read((char*)&ch, 2);
				if (format == "big")
				{
					SwapOrder((char*)&ch, 2);
				}
				num = ch;
			}
			else if (szOfNumInFace == 4)
			{
				int ch;
				in.read((char*)&ch, 4);
				if (format == "big")
				{
					SwapOrder((char*)&ch, 4);
				}
				num = ch;
			}
			vector<int> indices(num);
			for (int j = 0; j < num; ++j)
			{
				int index(0);
				if (szOfVertIndex == 2)
				{
					short ch;
					in.read((char*)&ch, 2);
					if (format == "big")
					{
						SwapOrder((char*)&ch, 2);
					}
					index = ch;
				}
				else if (szOfVertIndex == 4)
				{
					int ch;
					in.read((char*)&ch, 4);
					if (format == "big")
					{
						SwapOrder((char*)&ch, 4);
					}
					index = ch;
				}
				indices[j] = vertCorrespondence[index];
			}

			for (int j = 1; j < num - 1; ++j)
			{
				m_Faces.push_back(CFace(indices[0], indices[j], indices[j + 1]));
			}
		}
	}
	else
	{
		if (szOfNumInFace == 1)
		{
			char ch;
			in.read((char*)&ch, 1);
			faceNum = ch;
		}
		else if (szOfNumInFace == 2)
		{
			short ch;
			in.read((char*)&ch, 2);
			if (format == "big")
			{
				SwapOrder((char*)&ch, 2);
			}
			faceNum = ch;
		}
		else if (szOfNumInFace == 4)
		{
			int ch;
			in.read((char*)&ch, 4);
			if (format == "big")
			{
				SwapOrder((char*)&ch, 4);
			}
			faceNum = ch;
		}
		vector<int> magicBox;
		int minPos(0);
		bool fInverse(false);
		for (int i = 0; i < faceNum; ++i)
		{
			int curIndex;
			if (szOfVertIndex == 1)
			{
				char ch;
				in.read((char*)&ch, 1);
				curIndex = ch;
			}
			else if (szOfVertIndex == 2)
			{
				short ch;
				in.read((char*)&ch, 2);
				if (format == "big")
				{
					SwapOrder((char*)&ch, 2);
				}
				curIndex = ch;
			}
			else if (szOfVertIndex == 4)
			{
				int ch;
				in.read((char*)&ch, 4);
				if (format == "big")
				{
					SwapOrder((char*)&ch, 4);
				}
				curIndex = ch;
			}

			if (curIndex == -1)
			{
				magicBox.clear();
				minPos = 0;
				fInverse = false;
				continue;
			}
			else
			{
				curIndex = vertCorrespondence[curIndex];
			}
			if (magicBox.size() < 3)
			{
				magicBox.push_back(curIndex);
			}
			else
			{
				magicBox[minPos] = curIndex;
				fInverse = !fInverse;
			}			
			minPos = (minPos + 1) % 3;
			if (magicBox.size() >= 3)
			{
				if (fInverse)
					m_Faces.push_back(CFace(magicBox[0], magicBox[2], magicBox[1]));
				else
					m_Faces.push_back(CFace(magicBox[0], magicBox[1], magicBox[2]));
			}
		}
	}
	in.close();
    vector<CFace> testF(m_Faces);
    m_Faces.swap(testF);
}

//eat off the remainder of the current line.
void CBaseModel::ReadUntilNextLine(ifstream& in) const
{
	if (in.eof())
		return;
	char ch;
	while (in.read(&ch, 1), !(ch == 0x0A || ch == 0x0D || in.eof()));
	if (in.eof())
		return;
	while (in.read(&ch, 1), (ch == 0x0A || ch == 0x0D) && !in.eof());
	if (in.eof())
		return;
	in.putback(ch);
}

void CBaseModel::SwapOrder(char *buf, int sz)
{
	for (int i = 0; i < sz / 2; ++i)
	{
		char temp = buf[i];
		buf[i] = buf[sz - 1 - i];
		buf[sz - 1 - i] = temp;
	}
}

void CBaseModel::ReadIntoWord(ifstream &in, string& word) const
{
	word.clear();
	char ch;
	while (in.read(&ch, 1), (ch == 0xA || ch == 0xD || ch == ' ' || ch == '\t' || ch == '\n') && !in.eof());
	while (!(ch == 0xA || ch == 0xD || ch == ' ' || ch == '\t' || ch == '\n') && !in.eof())
	{
		word.push_back(ch);
		in.read(&ch, 1);
	}
	if (in.eof())
		return;
	in.putback(ch);
}
