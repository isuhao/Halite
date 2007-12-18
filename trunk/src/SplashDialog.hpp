
//         Copyright E�in O'Callaghan 2006 - 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#ifndef HALITE_MINI
#	include "Halite.hpp"
#else
#	include "HaliteMini.hpp"
#endif

#include "halTorrent.hpp"

class SplashDialog :
	public CDialogImpl<SplashDialog>,
	public CWinDataExchange<SplashDialog>
{
protected:
	typedef HaliteDialog thisClass;
	typedef CDialogImpl<SplashDialog> baseClass;

public:
	enum { IDD = IDD_CLOSESPLASH };

    BEGIN_MSG_MAP_EX(CMainDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    END_MSG_MAP()
	
	BEGIN_DDX_MAP(CMainDlg)
        DDX_CHECK(IDC_SPLASH_MSG, halite().showMessage)
    END_DDX_MAP()
	
	LRESULT SplashDialog::OnInitDialog(...)
	{
		CenterWindow();
		DoDataExchange(false);
		
		thread_ptr.reset(new thread(bind(&SplashDialog::SplashThread, this)));
		
		return TRUE;
	}

	void SplashThread()
	{
		hal::bittorrent().closeAll();
		hal::bittorrent().shutDownSession();
		
		DoDataExchange(true);
		EndDialog(0);
	}
	
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return this->IsDialogMessage(pMsg);
	}
	
private:
	boost::scoped_ptr<thread> thread_ptr;
};