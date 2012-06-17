
// SlotDescriptorView.cpp : implementation of the CSlotDescriptorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SWGEd.h"
#endif

#include <memory>
#include <sstream>

#include "TreDoc.h"
#include "SlotDescriptorView.h"

#include "meshLib/iff/sltd.hpp"

#using <PresentationFramework.dll>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace swf = System::Windows::Forms;

// CDatatableView


ref class CSlotDescriptorViewWrapper
{
    CSlotDescriptorView* view_;

public:
    CSlotDescriptorViewWrapper(CSlotDescriptorView* view)
        : view_(view)
    {}
    
    void OnDataError(System::Object^ object, swf::DataGridViewDataErrorEventArgs^ e)
    {}
};

IMPLEMENT_DYNCREATE(CSlotDescriptorView, Microsoft::VisualC::MFC::CWinFormsView)

BEGIN_MESSAGE_MAP(CSlotDescriptorView, Microsoft::VisualC::MFC::CWinFormsView)
	// Standard printing commands
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CDatatableView construction/destruction

CSlotDescriptorView::CSlotDescriptorView()
    : Microsoft::VisualC::MFC::CWinFormsView(DoubleBufferedDataGridView::typeid)
{
	// TODO: add construction code here

}

CSlotDescriptorView::~CSlotDescriptorView()
{
}

BOOL CSlotDescriptorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CDatatableView drawing

BOOL CSlotDescriptorView::OnEraseBackground(CDC* pDC)
{
    CBrush brNew(RGB(0,0,255));  //Creates a blue brush
    CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brNew);
     
    CRect rc;
    pDC->GetClipBox(rc); // Gets the co-ordinates of the client
                         // area to repaint.
    pDC->PatBlt(0,0,rc.Width(),rc.Height(),PATCOPY);
                         // Repaints client area with current brush.
    pDC->SelectObject(pOldBrush);
     
    return TRUE;    // Prevents the execution of return
                    // CView::OnEraseBkgnd(pDC) statement
}

void CSlotDescriptorView::OnDraw(CDC* /*pDC*/)
{
	CTreDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    if (!pDoc || pDoc->GetLength() == 0)
		return;



	// TODO: add draw code for native data here
    GetControl()->Refresh();
}


// CDatatableView printing
DoubleBufferedDataGridView^ CSlotDescriptorView::GetControl()
{
	System::Windows::Forms::Control^ control = CWinFormsView::GetControl();
	return safe_cast<DoubleBufferedDataGridView^>(control);
}

void CSlotDescriptorView::OnInitialUpdate()
{
	CTreDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    if (!pDoc || pDoc->GetLength() == 0)
		return;
    
    auto wrapper = gcnew CSlotDescriptorViewWrapper(this);
    auto control = GetControl();
    control->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    control->Dock = System::Windows::Forms::DockStyle::Fill;
    control->DataError += gcnew swf::DataGridViewDataErrorEventHandler(wrapper, &CSlotDescriptorViewWrapper::OnDataError);
    control->BorderStyle = swf::BorderStyle::None;
    control->BackgroundColor = System::Drawing::Color::White;
    control->ColumnHeadersDefaultCellStyle->Padding = swf::Padding(5, 2, 2, 5);
    control->SelectionMode = swf::DataGridViewSelectionMode::FullRowSelect;
    control->ColumnHeadersBorderStyle = swf::DataGridViewHeaderBorderStyle::Single;
    control->CellBorderStyle = swf::DataGridViewCellBorderStyle::Single;
    control->GridColor = System::Drawing::SystemColors::ActiveBorder;

    control->MultiSelect = false;
    
    swf::DataGridViewColumn^ column = gcnew swf::DataGridViewColumn(gcnew swf::DataGridViewTextBoxCell());
    
    column->HeaderText = gcnew System::String("Slot");
    column->Name = gcnew System::String("Slot");
    column->SortMode = swf::DataGridViewColumnSortMode::Automatic;
    column->MinimumWidth = column->Name->Length * 10;
    control->Columns->Add(column);
    
    // Read file data into a stream
    auto file_data = pDoc->GetData();
    auto iffFile = std::stringstream();
    iffFile.write(reinterpret_cast<char*>(&file_data[0]), file_data.size());
    
    ml::iff::sltd sltd;
    sltd.readSLTD(iffFile);

    auto slots = sltd.getSlots();

    for(auto& slot : slots)
    {
        swf::DataGridViewRow^ data_row = gcnew swf::DataGridViewRow();
        swf::DataGridViewCell^ cell = gcnew swf::DataGridViewTextBoxCell();
        cell->Value = gcnew System::String(slot.c_str());
        
        int length = slot.length() * 6;
        if (length > control->Columns[0]->MinimumWidth)
            control->Columns[0]->MinimumWidth = length;
        
        data_row->Cells->Add(cell);
        control->Rows->Add(data_row);
    }
}

void CSlotDescriptorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSlotDescriptorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSlotDescriptorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSlotDescriptorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSlotDescriptorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSlotDescriptorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDatatableView diagnostics

#ifdef _DEBUG
void CSlotDescriptorView::AssertValid() const
{
	CView::AssertValid();
}

void CSlotDescriptorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTreDoc* CSlotDescriptorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTreDoc)));
	return (CTreDoc*)m_pDocument;
}
#endif //_DEBUG


// CDatatableView message handlers
