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
// ExactMethodForDGP.h: interface for the CExactMethodForDGP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXACTMETHODFORDGP_H__2D7CA115_02A8_4808_838A_AE74834C0D93__INCLUDED_)
#define AFX_EXACTMETHODFORDGP_H__2D7CA115_02A8_4808_838A_AE74834C0D93__INCLUDED_

#include "RichModel.h"
#include <cfloat>
class CExactMethodForDGP  
{
public:
	struct InfoAtVertex
	{
		bool fParentIsPseudoSource;
		char birthTime;		
		int indexOfParent;
		int indexOfRootVertOfParent;
		int level;
		double disUptodate;
		double entryProp;

		InfoAtVertex()
		{
			birthTime = -1;
			disUptodate = DBL_MAX;
		}		
	};
	struct QuoteInfoAtVertex
	{
		char birthTime;
		int indexOfVert;
		double disUptodate;
		bool operator<(const QuoteInfoAtVertex& another) const
		{
			return disUptodate > another.disUptodate;
		}
		QuoteInfoAtVertex(){}
		QuoteInfoAtVertex(char birthTime, int indexOfVert, double disUptodate)
		{
			this->birthTime = birthTime;
			this->indexOfVert = indexOfVert;
			this->disUptodate = disUptodate;
		}
	};
	void BackTrace(int indexOfVert, vector<CPoint3D>& resultpoints);

protected:	
	bool fComputationCompleted;	
	bool fLocked;
	double totalLen;
	int nTotalCurves;

	int indexOfSourceVert;
	int nCountOfWindows;
	double nTotalMilliSeconds;
	int nMaxLenOfWindowQueue;
	double nMaxLenOfPseudoSources;
	int depthOfResultingTree;
	double NPE;
	double memory;
	double farestDis;
	const CRichModel& model;
	string nameOfAlgorithm;
protected:
	vector<InfoAtVertex> m_InfoAtVertices;
public:
	CExactMethodForDGP(const CRichModel& inputModel, int indexOfSourceVert = 0);
	virtual ~CExactMethodForDGP();
	virtual void Execute();
	virtual void InitContainers() = 0;
	virtual void BuildSequenceTree() = 0;
	virtual void ClearContainers() = 0;
	virtual void FillExperimentalResults() = 0;
	inline double GetRunTime() const;
	inline double GetMemoryCost() const;
	inline int GetWindowNum() const;
	inline int GetMaxLenOfQue() const;
	inline double GetNPE() const;
	inline int GetDepthOfSequenceTree() const;
	inline string GetAlgorithmName() const;
	inline bool HasBeenCompleted() const;
};

double CExactMethodForDGP::GetRunTime() const
{
	return nTotalMilliSeconds;
}

double CExactMethodForDGP::GetMemoryCost() const
{
	return memory;
}

int CExactMethodForDGP::GetWindowNum() const
{
	return nCountOfWindows;
}

int CExactMethodForDGP::GetMaxLenOfQue() const
{
	return nMaxLenOfWindowQueue;
}

int CExactMethodForDGP::GetDepthOfSequenceTree() const
{
	return depthOfResultingTree;
}

double CExactMethodForDGP::GetNPE() const
{
	return NPE;
}

string CExactMethodForDGP::GetAlgorithmName() const
{
	return nameOfAlgorithm;
}

bool CExactMethodForDGP::HasBeenCompleted() const
{
	return fComputationCompleted;
}
#endif // !defined(AFX_EXACTMETHODFORDGP_H__2D7CA115_02A8_4808_838A_AE74834C0D93__INCLUDED_)
