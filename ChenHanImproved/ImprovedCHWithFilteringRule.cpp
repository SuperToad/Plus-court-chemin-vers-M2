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
// ImprovedCHWithEdgeValve.cpp: implementation of the CImprovedCHWithFilteringRule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImprovedCHWithFilteringRule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImprovedCHWithFilteringRule::CImprovedCHWithFilteringRule(const CRichModel& inputModel, int indexOfSourceVert) : CPreviousCH(inputModel, indexOfSourceVert)
{
	nameOfAlgorithm = "ICH1";
}

CImprovedCHWithFilteringRule::~CImprovedCHWithFilteringRule()
{

}


bool CImprovedCHWithFilteringRule::CheckValidityOfWindow(Window& w)
{
	if (w.fDirectParenIsPseudoSource)
		return true;
	const CRichModel::CEdge& edge = model.Edge(w.indexOfCurEdge);

	int leftVert = edge.indexOfLeftVert;
	double detaX = w.xUponUnfolding - w.proportions[1] * edge.length;
	double rightLen = sqrt(detaX * detaX + w.yUponUnfolding * w.yUponUnfolding);
	if (m_InfoAtVertices[leftVert].disUptodate < 10000 && m_InfoAtVertices[leftVert].disUptodate + w.proportions[1] * edge.length
		< w.disToRoot + rightLen)
	{
		return false;
	}
	int rightVert = edge.indexOfRightVert;
	detaX = w.xUponUnfolding - w.proportions[0] * edge.length;
	double leftLen = sqrt(detaX * detaX + w.yUponUnfolding * w.yUponUnfolding);
	if (m_InfoAtVertices[rightVert].disUptodate < 10000 && m_InfoAtVertices[rightVert].disUptodate + (1 - w.proportions[0]) * edge.length
		< w.disToRoot + leftLen)
	{
		return false;
	}
	const CRichModel::CEdge& oppositeEdge = model.Edge(edge.indexOfReverseEdge);
	double xOfVert = edge.length - oppositeEdge.xOfPlanarCoordOfOppositeVert;
	double yOfVert = -oppositeEdge.yOfPlanarCoordOfOppositeVert;
	if (m_InfoAtVertices[oppositeEdge.indexOfOppositeVert].disUptodate < 10000)	
	{
		if (w.fDirectParentEdgeOnLeft)
		{
			double deta = w.disToRoot + leftLen - m_InfoAtVertices[oppositeEdge.indexOfOppositeVert].disUptodate;
			if (deta <= 0)
				return true;
			detaX = xOfVert - w.proportions[0] * edge.length;
			if (detaX * detaX + yOfVert * yOfVert < deta * deta)
				return false;
		}
		else
		{
			double deta = w.disToRoot + rightLen - m_InfoAtVertices[oppositeEdge.indexOfOppositeVert].disUptodate;
			if (deta <= 0)
				return true;
			detaX = xOfVert - w.proportions[1] * edge.length;
			if (detaX * detaX + yOfVert * yOfVert < deta * deta)
				return false;
		}	
	}
	return true;
}
