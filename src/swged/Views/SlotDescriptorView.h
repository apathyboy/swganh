
// SlotDescriptorView.h : interface of the CSlotDescriptorView class
//

#pragma once

#include <afxwinforms.h>

#include "DoubleBufferedGridView.h"

class CSlotDescriptorView : public Microsoft::VisualC::MFC::CWinFormsView
{
protected: // create from serialization only
	CSlotDescriptorView();
	DECLARE_DYNCREATE(CSlotDescriptorView)

// Attributes
public:
	CTreDoc* GetDocument() const;
	DoubleBufferedDataGridView^ GetControl();

// Operations
public:

// Overrides
public:
    virtual BOOL OnEraseBackground(CDC* pDC);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    void OnDataError(System::Object^ object, System::EventArgs^ e);
protected:
   	virtual void OnInitialUpdate();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSlotDescriptorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CSlotDescriptorView.cpp
inline CTreDoc* CSlotDescriptorView::GetDocument() const
   { return reinterpret_cast<CTreDoc*>(m_pDocument); }
#endif

