// OpenSceneGraphView.h : interface of the COpenSceneGraphView class
//
#pragma once

#include "MFCOpenSceneGraph.h"

class COpenSceneGraphView : public CView
{
protected: // create from serialization only
    COpenSceneGraphView();
    DECLARE_DYNCREATE(COpenSceneGraphView)

// Attributes
public:
    CTreDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnInitialUpdate();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
    virtual ~COpenSceneGraphView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CMFCOpenSceneGraph* mOSG;
    //HANDLE mThreadHandle;
    CRenderingThread* mThreadHandle;

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in OpenSceneGraphView.cpp
inline CTreDoc* COpenSceneGraphView::GetDocument() const
   { return reinterpret_cast<CTreDoc*>(m_pDocument); }
#endif

