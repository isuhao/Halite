
//         Copyright Eoin O'Callaghan 2006 - 2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define ID_ADD_TORRENT_BEGIN	 	11000
#define IDC_ADDT_MOVETO_FOLDER		ID_ADD_TORRENT_BEGIN + 1
#define IDC_ADDT_MOVETO_BROWSE		ID_ADD_TORRENT_BEGIN + 2
#define IDC_ADDT_MOVETO_CHECK		ID_ADD_TORRENT_BEGIN + 3
#define IDC_ADDT_DEFFLD_TEXT		ID_ADD_TORRENT_BEGIN + 6
#define IDC_ADDT_NOTE_TEXT			ID_ADD_TORRENT_BEGIN + 8

#ifndef RC_INVOKED

#include <boost/function.hpp>
#include "halTorrent.hpp"

class AddTorrentDialog :
	public CDialogImpl<AddTorrentDialog>,
	public CAutoSizeWindow<AddTorrentDialog, true>,
    public CWinDataExchangeEx<AddTorrentDialog>
{
protected:
	typedef AddTorrentDialog thisClass;
	typedef CDialogImpl<thisClass> baseClass;
	typedef CAutoSizeWindow<thisClass, true> autosizeClass;

public:
	AddTorrentDialog(wstring& d, wstring& m, bool& u, bool& p, bool& c) :
		saveDirectory_(d),
		moveToDirectory_(m),
		useMove_(u),
		startPaused_(p),
		compactStorage_(c)
	{}
	
	enum { IDD = IDD_ADD_TORRENT };

    BEGIN_MSG_MAP_EX(thisClass)
        MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		COMMAND_ID_HANDLER_EX(IDC_ADDT_MOVETO_CHECK, OnMoveTo)
		COMMAND_ID_HANDLER_EX(IDC_BC_SAVEBROWSE, OnBrowse)
		COMMAND_ID_HANDLER_EX(IDC_ADDT_MOVETO_BROWSE, OnMoveBrowse)
		
        CHAIN_MSG_MAP(autosizeClass)
    END_MSG_MAP()
	
    BEGIN_DDX_MAP(thisClass)
        DDX_CHECK(IDC_CHECK_COMPACT, compactStorage_)
        DDX_CHECK(IDC_CHECK_PAUSED, startPaused_)
        DDX_CHECK(IDC_ADDT_MOVETO_CHECK, useMove_)
		DDX_EX_STDWSTRING(IDC_BC_SAVEFOLDER, saveDirectory_)
		DDX_EX_STDWSTRING(IDC_ADDT_MOVETO_FOLDER, moveToDirectory_)
    END_DDX_MAP()	

#define ADD_FOLDERS_LAYOUT \
	WMB_HEAD(WMB_COLNOMAX(_exp), WMB_COL(_auto)), \
		WMB_ROW(_auto,	IDC_ADDT_DEFFLD_TEXT, _r), \
		WMB_ROW(_auto,	IDC_BC_SAVEFOLDER, IDC_BC_SAVEBROWSE), \
		WMB_ROW(_auto,	IDC_ADDT_MOVETO_CHECK, _r), \
		WMB_ROW(_auto,	IDC_ADDT_MOVETO_FOLDER, IDC_ADDT_MOVETO_BROWSE), \
	WMB_END()

#define ADD_BUTTONS_LAYOUT \
	WMB_HEAD(WMB_COLNOMAX(_exp), WMB_COL(_auto), WMB_COL(_auto)), \
		WMB_ROW(_auto,	0, IDOK, IDCANCEL), \
	WMB_END()

	BEGIN_WINDOW_MAP(thisClass, 6, 6, 3, 3)
		WMB_HEAD(WMB_COLNOMAX(_exp)),
			WMB_ROW(_auto,	ADD_FOLDERS_LAYOUT), 
			WMB_ROW(_gap,	_d),
			WMB_ROW(_auto,	IDC_CHECK_COMPACT), 
			WMB_ROW(_auto,	IDC_CHECK_PAUSED), 
			WMB_ROW(_auto,	IDC_ADDT_NOTE_TEXT), 
			WMB_ROW(_gap,	_d),
			WMB_ROW(_auto,	ADD_BUTTONS_LAYOUT), 
		WMB_END()
	END_WINDOW_MAP()	

	LRESULT OnInitDialog(HWND hwndFocus, LPARAM lParam)
	{
		CenterWindow();		

		CtrlsInitialize();
		CtrlsArrange();
		
		BOOL retval =  DoDataExchange(false);

		OnMoveTo(0, 0, GetDlgItem(IDC_ADDT_MOVETO_CHECK));
		return retval;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return this->IsDialogMessage(pMsg);
	}
	
	void OnCancel(UINT, int, HWND hWnd)
	{
		EndDialog(IDCANCEL);
	}
	
	void OnOk(UINT, int, HWND hWnd)
	{
		DoDataExchange(true);
		EndDialog(IDOK);
	}
	
	void OnMoveTo(UINT, int, HWND hWnd)
	{
		LRESULT result = ::SendMessage(hWnd, BM_GETCHECK, 0, 0);

		if (result == BST_CHECKED)
		{
			::EnableWindow(GetDlgItem(IDC_ADDT_MOVETO_FOLDER), true);
			::EnableWindow(GetDlgItem(IDC_ADDT_MOVETO_BROWSE), true);
		}
		else
		{
			::EnableWindow(GetDlgItem(IDC_ADDT_MOVETO_FOLDER), false);
			::EnableWindow(GetDlgItem(IDC_ADDT_MOVETO_BROWSE), false);
		}
	}
	
	void OnBrowse(UINT, int, HWND hWnd)
	{
		std::wstring save_prompt = hal::app().res_wstr(IDS_SAVEPROMPT);		
		CFolderDialog fldDlg(NULL, save_prompt.c_str(),
			BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE);
		
		wstring defaultSaveFolder = saveDirectory_;
		fldDlg.SetInitialFolder(defaultSaveFolder.c_str());
	 
		if (IDOK == fldDlg.DoModal())
		{
			saveDirectory_ = wstring(fldDlg.m_szFolderPath);
			DoDataExchange(false);
		}
	}
	
	void OnMoveBrowse(UINT, int, HWND hWnd)
	{
		std::wstring save_prompt = hal::app().res_wstr(IDS_SAVEPROMPT);		
		CFolderDialog fldDlg(NULL, save_prompt.c_str(),
			BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE);
		
		wstring defaultMoveFolder = moveToDirectory_;
		fldDlg.SetInitialFolder(defaultMoveFolder.c_str());
	 
		if (IDOK == fldDlg.DoModal())
		{
			moveToDirectory_ = wstring(fldDlg.m_szFolderPath);
			DoDataExchange(false);
		}
	}
	
private:
	wstring& saveDirectory_;
	wstring& moveToDirectory_;
	bool& useMove_;
	bool& startPaused_;
	bool& compactStorage_;
};

#endif // RC_INVOKED
