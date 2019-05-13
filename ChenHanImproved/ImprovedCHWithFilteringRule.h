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
// ImprovedCHWithEdgeValve.h: interface for the CImprovedCHWithFilteringRule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPROVEDCHWITHEDGEVALVE_H__9012603F_B3CB_47E3_9F8A_5B746411D0A3__INCLUDED_)
#define AFX_IMPROVEDCHWITHEDGEVALVE_H__9012603F_B3CB_47E3_9F8A_5B746411D0A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PreviousCH.h"
class CImprovedCHWithFilteringRule : public CPreviousCH 
{
private:
	//ofstream out;
protected:
	virtual bool CheckValidityOfWindow(Window& w);
public:
	CImprovedCHWithFilteringRule(const CRichModel& inputModel, int indexOfSourceVert = 0);
	virtual ~CImprovedCHWithFilteringRule();
};

#endif // !defined(AFX_IMPROVEDCHWITHEDGEVALVE_H__9012603F_B3CB_47E3_9F8A_5B746411D0A3__INCLUDED_)
