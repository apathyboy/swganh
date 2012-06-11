
// SWGEd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SWGEd.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#include "TreDoc.h"
#include "HexView.h"
#include "TreDocManager.h"
#include "Views/DatatableView.h"
#include "Views/OpenSceneGraphView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSWGEdApp

BEGIN_MESSAGE_MAP(CSWGEdApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSWGEdApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSWGEdApp construction

CSWGEdApp::CSWGEdApp()
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("SWGEd.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSWGEdApp object

CSWGEdApp theApp;


// CSWGEdApp initialization

BOOL CSWGEdApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	//LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    m_pDocManager = new CTreDocManager;
    
    osg_doc_template_ = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTreDoc),
		RUNTIME_CLASS(CChildFrame),       // main SDI frame window
		RUNTIME_CLASS(COpenSceneGraphView));
    if (!osg_doc_template_)
        return FALSE;
	AddDocTemplate(osg_doc_template_);

    datatable_doc_template_ = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTreDoc),
		RUNTIME_CLASS(CChildFrame),       // main SDI frame window
		RUNTIME_CLASS(CDatatableView));
    if (!datatable_doc_template_)
        return FALSE;
	AddDocTemplate(datatable_doc_template_);

	hex_doc_template_ = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTreDoc),
		RUNTIME_CLASS(CChildFrame),       // main SDI frame window
		RUNTIME_CLASS(CHexView));
    if (!hex_doc_template_)
        return FALSE;
	AddDocTemplate(hex_doc_template_);
            
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
    if ( cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew )
    {
        cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing ;
    }

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CSWGEdApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CSWGEdApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSWGEdApp customization load/save methods

void CSWGEdApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CSWGEdApp::LoadCustomState()
{
}

void CSWGEdApp::SaveCustomState()
{
}

CMultiDocTemplate* CSWGEdApp::GetHexDocTemplate()
{
    return hex_doc_template_;
}

CMultiDocTemplate* CSWGEdApp::GetDatatableDocTemplate()
{
    return datatable_doc_template_;
}

CMultiDocTemplate* CSWGEdApp::GetOSGDocTemplate()
{
    return osg_doc_template_;
}

// CSWGEdApp message handlers



