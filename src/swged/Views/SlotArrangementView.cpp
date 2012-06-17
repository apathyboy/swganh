
// SlotArrangementView.cpp : implementation of the CSlotArrangementView class
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
#include "SlotArrangementView.h"

#include "meshLib/iff/argd.hpp"

#using <PresentationFramework.dll>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace swf = System::Windows::Forms;

// CDatatableView


ref class CSlotArrangementViewWrapper
{
    CSlotArrangementView* view_;

public:
    CSlotArrangementViewWrapper(CSlotArrangementView* view)
        : view_(view)
    {}
    
    void OnDataError(System::Object^ object, swf::DataGridViewDataErrorEventArgs^ e)
    {}
};

IMPLEMENT_DYNCREATE(CSlotArrangementView, Microsoft::VisualC::MFC::CWinFormsView)

BEGIN_MESSAGE_MAP(CSlotArrangementView, Microsoft::VisualC::MFC::CWinFormsView)
	// Standard printing commands
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CDatatableView construction/destruction

CSlotArrangementView::CSlotArrangementView()
    : Microsoft::VisualC::MFC::CWinFormsView(DoubleBufferedDataGridView::typeid)
{
	// TODO: add construction code here

}

CSlotArrangementView::~CSlotArrangementView()
{
}

BOOL CSlotArrangementView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSlotArrangementView drawing

BOOL CSlotArrangementView::OnEraseBackground(CDC* pDC)
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

void CSlotArrangementView::OnDraw(CDC* /*pDC*/)
{
	CTreDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    if (!pDoc || pDoc->GetLength() == 0)
		return;



	// TODO: add draw code for native data here
    GetControl()->Refresh();
}


// CDatatableView printing
DoubleBufferedDataGridView^ CSlotArrangementView::GetControl()
{
	System::Windows::Forms::Control^ control = CWinFormsView::GetControl();
	return safe_cast<DoubleBufferedDataGridView^>(control);
}

void CSlotArrangementView::OnInitialUpdate()
{
	CTreDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
    if (!pDoc || pDoc->GetLength() == 0)
		return;
    
    auto wrapper = gcnew CSlotArrangementViewWrapper(this);
    auto control = GetControl();
    control->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    control->Dock = System::Windows::Forms::DockStyle::Fill;
    control->DataError += gcnew swf::DataGridViewDataErrorEventHandler(wrapper, &CSlotArrangementViewWrapper::OnDataError);
    control->BorderStyle = swf::BorderStyle::None;
    control->BackgroundColor = System::Drawing::Color::White;
    control->ColumnHeadersDefaultCellStyle->Padding = swf::Padding(5, 2, 2, 5);
    control->SelectionMode = swf::DataGridViewSelectionMode::FullRowSelect;
    control->ColumnHeadersBorderStyle = swf::DataGridViewHeaderBorderStyle::Single;
    control->CellBorderStyle = swf::DataGridViewCellBorderStyle::Single;
    control->GridColor = System::Drawing::SystemColors::ActiveBorder;

    control->MultiSelect = false;
    
    swf::DataGridViewColumn^ column = gcnew swf::DataGridViewColumn(gcnew swf::DataGridViewTextBoxCell());
    //column->DefaultCellStyle = swf::DataGridViewColumn::DefaultCellStyle
    column->HeaderText = gcnew System::String("Slot Arrangement");
    column->Name = gcnew System::String("Slot Arrangement");
    column->SortMode = swf::DataGridViewColumnSortMode::Automatic;
    column->MinimumWidth = column->Name->Length * 10;
    column->AutoSizeMode = swf::DataGridViewAutoSizeColumnMode::AllCells;
    control->Columns->Add(column);
    
    // Read file data into a stream
    auto file_data = pDoc->GetData();
    auto iffFile = std::stringstream();
    iffFile.write(reinterpret_cast<char*>(&file_data[0]), file_data.size());
    
    ml::iff::argd argd;
    argd.readARGD(iffFile);
    
    auto slot_arrangements = argd.getSlotArrangements();
    
    for(auto& arrangement : slot_arrangements)
    {
        if (arrangement.size() == 0)
            continue;

        swf::DataGridViewRow^ data_row = gcnew swf::DataGridViewRow();
        swf::DataGridViewCell^ cell = gcnew swf::DataGridViewTextBoxCell();

        int max_length;
        std::string arrangement_text;

        std::tie(arrangement_text, max_length) = BuildArrangementCellText(arrangement);
        cell->Style->WrapMode = swf::DataGridViewTriState::True;
        cell->Value = gcnew System::String(arrangement_text.c_str());
        
        data_row->Cells->Add(cell);
        data_row->MinimumHeight = data_row->Height * arrangement.size();
        control->Rows->Add(data_row);
    }
}

std::tuple<std::string, int> CSlotArrangementView::BuildArrangementCellText(const std::vector<std::string>& arrangement)
{
    int max_length = 0;
    std::string output;
    for (auto& slot : arrangement)
    {
        output += slot + "\n";

        if (slot.length() > static_cast<unsigned int>(max_length))
            max_length = slot.length();
    }

    return std::make_tuple(output, max_length);
}

void CSlotArrangementView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSlotArrangementView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSlotArrangementView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSlotArrangementView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CSlotArrangementView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSlotArrangementView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSlotArrangementView diagnostics

#ifdef _DEBUG
void CSlotArrangementView::AssertValid() const
{
	CView::AssertValid();
}

void CSlotArrangementView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTreDoc* CSlotArrangementView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTreDoc)));
	return (CTreDoc*)m_pDocument;
}
#endif //_DEBUG


// CSlotArrangementView message handlers
