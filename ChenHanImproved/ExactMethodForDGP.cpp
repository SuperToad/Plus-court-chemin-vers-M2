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
// ExactMethodForDGP.cpp: implementation of the CExactMethodForDGP class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExactMethodForDGP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExactMethodForDGP::CExactMethodForDGP(const CRichModel& inputModel, int indexOfSourceVert) : model(inputModel)
{
	this->indexOfSourceVert = indexOfSourceVert;
	nCountOfWindows = 0;
	nMaxLenOfPseudoSources = 0;
	nMaxLenOfWindowQueue = 0;
	depthOfResultingTree = 0;
	totalLen = 0;
	fComputationCompleted = false;
	fLocked = false;
	NPE = 0;
	memory = 0;
	nTotalCurves = 0;
	nameOfAlgorithm = "";
	m_InfoAtVertices.resize(model.GetNumOfVerts());
	memory += double(model.GetNumOfVerts()) * sizeof(InfoAtVertex) / 1024 / 1024;
}

CExactMethodForDGP::~CExactMethodForDGP()
{
}

void CExactMethodForDGP::BackTrace(int indexOfVert, vector<CPoint3D>& resultpoints)
{
	if (m_InfoAtVertices[indexOfVert].birthTime == -1)
	{
		assert(model.GetNumOfComponents() != 1 || model.Neigh(indexOfVert).empty());
		return;
	}
	resultpoints.clear();
	vector<int> vertexNodes;
	int index = indexOfVert;
	vertexNodes.push_back(index);
	while (index != indexOfSourceVert)
	{
		int indexOfParent = m_InfoAtVertices[index].indexOfParent;
		if (m_InfoAtVertices[index].fParentIsPseudoSource)
		{
			index = indexOfParent;
		}
		else
		{
			index = m_InfoAtVertices[index].indexOfRootVertOfParent;
		}
		vertexNodes.push_back(index);
	};


	for (int i = 0; i < (int)vertexNodes.size() - 1; ++i)
	{
		int lastVert = vertexNodes[i];
		CPoint3D pt = model.ComputeShiftPoint(lastVert);
		resultpoints.push_back(pt);
		
		if (m_InfoAtVertices[lastVert].fParentIsPseudoSource)
		{
			continue;
		}
		int parentEdgeIndex = m_InfoAtVertices[lastVert].indexOfParent;
		int edgeIndex = model.Edge(parentEdgeIndex).indexOfReverseEdge;
		double leftLen = model.Edge(model.Edge(parentEdgeIndex).indexOfRightEdge).length;
		double rightLen = model.Edge(model.Edge(parentEdgeIndex).indexOfLeftEdge).length;
		double xBack = model.Edge(parentEdgeIndex).length - model.Edge(parentEdgeIndex).xOfPlanarCoordOfOppositeVert;
		double yBack = -model.Edge(parentEdgeIndex).yOfPlanarCoordOfOppositeVert;
		double disToAngle = model.DistanceToIncidentAngle(edgeIndex, xBack, yBack);
		
		double proportion = 1 - m_InfoAtVertices[lastVert].entryProp;
		while (1) 
		{
			CPoint3D pt1 = model.ComputeShiftPoint(model.Edge(edgeIndex).indexOfLeftVert);
			CPoint3D pt2 = model.ComputeShiftPoint(model.Edge(edgeIndex).indexOfRightVert);
			CPoint3D ptIntersection = CRichModel::CombineTwoNormalsTo(pt1, 1 - proportion, pt2, proportion);			
			resultpoints.push_back(ptIntersection);

			if (model.Edge(edgeIndex).indexOfOppositeVert == vertexNodes[i + 1])
				break;
			double oldProprotion = proportion;
			proportion = model.ProportionOnLeftEdgeByImage(edgeIndex,xBack, yBack, oldProprotion);
			if (proportion >= -LENGTH_EPSILON_CONTROL && proportion <= 1)
			{
				proportion = max<double>(proportion, 0);
				edgeIndex = model.Edge(edgeIndex).indexOfLeftEdge;
				rightLen = disToAngle;				
			}
			else
			{
				proportion = model.ProportionOnRightEdgeByImage(edgeIndex, xBack, yBack, oldProprotion);
				proportion = max<double>(proportion, 0);
				proportion = min<double>(proportion, 1);
				edgeIndex = model.Edge(edgeIndex).indexOfRightEdge;
				leftLen = disToAngle;				
			}
			model.GetPointByRotatingAround(edgeIndex, leftLen, rightLen, xBack, yBack);			
			disToAngle = model.DistanceToIncidentAngle(edgeIndex, xBack, yBack);
		};
	}
	resultpoints.push_back(model.ComputeShiftPoint(indexOfSourceVert));	
}

void CExactMethodForDGP::Execute()
{
	if (fComputationCompleted)
		return;
	if (!fLocked)
	{
		fLocked = true;
		nCountOfWindows = 0;	
		nMaxLenOfWindowQueue = 0;
		depthOfResultingTree = 0;
		InitContainers();
		nTotalMilliSeconds = clock();	
		BuildSequenceTree();
		nTotalMilliSeconds = clock() - nTotalMilliSeconds;
		FillExperimentalResults();
		ClearContainers();
		fComputationCompleted = true;
		fLocked = false;
	}
}