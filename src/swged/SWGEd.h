
// SWGEd.h : main header file for the SWGEd application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSWGEdApp:
// See SWGEd.cpp for the implementation of this class
//

class CSWGEdApp : public CWinAppEx
{
public:
	CSWGEdApp();

    CMultiDocTemplate* GetHexDocTemplate();
    CMultiDocTemplate* GetDatatableDocTemplate();
    CMultiDocTemplate* GetOSGDocTemplate();
    CMultiDocTemplate* GetSlotDescriptorDocTemplate();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;
    CMultiDocTemplate* hex_doc_template_;
    CMultiDocTemplate* datatable_doc_template_;
    CMultiDocTemplate* osg_doc_template_;
    CMultiDocTemplate* slot_descriptor_doc_template_;

public:
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
};

extern CSWGEdApp theApp;
