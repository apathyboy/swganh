// MFC_OSG_MDIView.cpp : implementation of the CMFC_OSG_MDIView class
//

#include "stdafx.h"
#include "SWGEd.h"
#include "TreDoc.h"
#include "OpenSceneGraphView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(COpenSceneGraphView, CView)

BEGIN_MESSAGE_MAP(COpenSceneGraphView, CView)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_KEYDOWN()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

COpenSceneGraphView::COpenSceneGraphView() :
   mOSG(0L)
{
}

COpenSceneGraphView::~COpenSceneGraphView()
{
}

BOOL COpenSceneGraphView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void COpenSceneGraphView::OnDraw(CDC* /*pDC*/)
{
    CTreDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;
}

#ifdef _DEBUG
void COpenSceneGraphView::AssertValid() const
{
    CView::AssertValid();
}

void COpenSceneGraphView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CTreDoc* COpenSceneGraphView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTreDoc)));
    return (CTreDoc*)m_pDocument;
}
#endif //_DEBUG


int COpenSceneGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    // Let MFC create the window before OSG
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Now that the window is created setup OSG
    mOSG = new CMFCOpenSceneGraph(m_hWnd, static_cast<CMainFrame*>(AfxGetMainWnd())->GetOpenSceneGraphRepository());

    return 1;
}

void COpenSceneGraphView::OnDestroy()
{
    delete mThreadHandle;
    if(mOSG != 0) delete mOSG;

    //WaitForSingleObject(mThreadHandle, 1000);

    CView::OnDestroy();
}

void COpenSceneGraphView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    // Get Filename from DocumentOpen Dialog
    CString csFileName = GetDocument()->GetFileName();
    //
    //// Init the osg class
    mOSG->InitOSG(csFileName.GetString());

    // Start the thread to do OSG Rendering
    //mThreadHandle = (HANDLE)_beginthread(&CMFCOpenSceneGraph::Render, 0, mOSG); 
    mThreadHandle = new CRenderingThread(mOSG);
    mThreadHandle->start();
}

void COpenSceneGraphView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Pass Key Presses into OSG
    mOSG->getViewer()->getEventQueue()->keyPress(nChar);

    // Close Window on Escape Key
    if(nChar == VK_ESCAPE)
    {
        GetParent()->SendMessage(WM_CLOSE);
    }
}


BOOL COpenSceneGraphView::OnEraseBkgnd(CDC* pDC)
{
    /* Do nothing, to avoid flashing on MSW */
    return true;
}
