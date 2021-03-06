
//         Copyright Eóin O'Callaghan 2006 - 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define ID_ADD_TORRENT_BEGIN	 		11000
#define HAL_ADJUST_DLG_HOLDER			ID_ADD_TORRENT_BEGIN + 1
#define HAL_ADDT_ALLOC_COMBO			ID_ADD_TORRENT_BEGIN + 2
#define HAL_ADDT_CHECK_MANAGED		ID_ADD_TORRENT_BEGIN + 3
#define HAL_ADDT_ALLOC_MODE			ID_ADD_TORRENT_BEGIN + 4
#define HAL_ADDT_MAGNET_TEXT			ID_ADD_TORRENT_BEGIN + 5
#define HAL_ADDT_MAGNET_URL			ID_ADD_TORRENT_BEGIN + 6


#ifndef RC_INVOKED

#include <boost/function.hpp>
#include "HaliteListViewDlg.hpp"
#include "halTorrent.hpp"

class AddTorrentDialog :
	public ATL::CDialogImpl<AddTorrentDialog>,
	public ATL::CAutoSizeWindow<AddTorrentDialog, true>,
    public WTLx::WinDataExchangeEx<AddTorrentDialog>,
	private hal::IniBase<AddTorrentDialog>
{
protected:
	typedef AddTorrentDialog this_class_t;
	typedef ATL::CDialogImpl<this_class_t> base_class_t;
	typedef ATL::CAutoSizeWindow<this_class_t, true> autosizeClass;
	typedef hal::IniBase<this_class_t> ini_class_t;

	typedef std::map<wstring, hal::bit::allocations> wstr2alloc_map;

public:
	AddTorrentDialog(wstring& name_or_uri, bool can_edit_name, wstring& d, wstring& m, bool& u, bool& p, bool& man, hal::bit::allocations& a_t) :	  
		ini_class_t(L"add_torrent_dialog", L"settings"),
		rect_(0,0,0,0),
		adjustDlg_(d, m, u),
		startPaused_(p),
		managed_(man),
		allocation_type_(a_t),
		name_or_uri_(name_or_uri),
		can_edit_name_(can_edit_name)
	{ 
		load_from_ini(); 
	}

	~AddTorrentDialog() 
	{ 
		save_to_ini(); 
	}	

	enum { IDD = HAL_ADD_TORRENT };

	BEGIN_MSG_MAP_EX(this_class_t)
		try
		{
		MSG_WM_INITDIALOG(OnInitDialog)

		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDOK, OnOk)
		}
		HAL_ALL_EXCEPTION_CATCH(L"in AddTorrentDialog MSG_MAP")

		CHAIN_MSG_MAP(autosizeClass)
	END_MSG_MAP()

	BEGIN_DDX_MAP(this_class_t)
		DDX_EX_STDWSTRING(HAL_ADDT_MAGNET_URL, name_or_uri_)
		DDX_CHECK(HAL_ADDT_CHECK_MANAGED, managed_)
		DDX_CHECK(HAL_CHECK_PAUSED, startPaused_)
		DDX_EX_STDWSTRING(HAL_ADDT_ALLOC_COMBO, allocation_type_name_)
	END_DDX_MAP()	

#define ADD_BUTTONS_LAYOUT \
	WMB_HEAD(WMB_COLNOMAX(_exp), WMB_COL(_auto), WMB_COL(_auto)), \
		WMB_ROW(_auto,	0, IDOK, IDCANCEL), \
	WMB_END()

#define ALLOC_MODE_LAYOUT \
	WMB_HEAD(WMB_COLNOMAX(_exp|33), WMB_COLNOMAX(_exp)), \
		WMB_ROW(_auto,	HAL_ADDT_ALLOC_MODE, HAL_ADDT_ALLOC_COMBO), \
	WMB_END()

#define TORRENT_NAME_OR_URI \
	WMB_HEAD(WMB_COLNOMAX(_exp|20), WMB_COLNOMAX(_exp)), \
		WMB_ROW(_auto,	HAL_ADDT_MAGNET_TEXT, HAL_ADDT_MAGNET_URL), \
	WMB_END()

	BEGIN_WINDOW_MAP(this_class_t, 6, 6, 3, 3)
		WMB_HEAD(WMB_COLNOMAX(_exp)),
			WMB_ROW(_auto,	TORRENT_NAME_OR_URI), 
			WMB_ROW(_exp,	HAL_ADJUST_DLG), 
			WMB_ROW(_gap,	_d),
			WMB_ROW(_auto,	ALLOC_MODE_LAYOUT), 
			WMB_ROW(_auto,	HAL_ADDT_CHECK_MANAGED), 
			WMB_ROW(_auto,	HAL_CHECK_PAUSED), 
			WMB_ROW(_auto,	HAL_ADDT_NOTE_TEXT), 
			WMB_ROW(_gap,	_d),
			WMB_ROW(_auto,	ADD_BUTTONS_LAYOUT), 
		WMB_END()
	END_WINDOW_MAP()	

	LRESULT OnInitDialog(HWND hwndFocus, LPARAM lParam)
	{
		if (rect_.left == rect_.right)
		{
			CenterWindow();
			GetWindowRect(rect_);
		}
		else 
		{
			MoveWindow(rect_.left, rect_.top, rect_.right-rect_.left, rect_.bottom-rect_.top, false);		
		}

		adjustDlg_.Create(*this);
		adjustDlg_.ShowWindow(SW_SHOW);
		adjustDlg_.SetDlgCtrlID(HAL_ADJUST_DLG);

		SendDlgItemMessage(HAL_ADDT_MAGNET_URL, WM_ENABLE, can_edit_name_ ? TRUE : FALSE, 0);

		CtrlsInitialize();
		CtrlsArrange();
		
		allocation_type_map_[hal::app().res_wstr(HAL_ALLOCATION_COMPACT)] = hal::bit::compact_allocation;
		allocation_type_map_[hal::app().res_wstr(HAL_ALLOCATION_FULL)] = hal::bit::full_allocation;
		allocation_type_map_[hal::app().res_wstr(HAL_ALLOCATION_SPARSE)] = hal::bit::sparse_allocation;

		BOOST_FOREACH (wstr2alloc_map::value_type i, allocation_type_map_)
		{
			::SendMessage(GetDlgItem(HAL_ADDT_ALLOC_COMBO), CB_ADDSTRING, 0, (LPARAM)i.first.c_str());
		}
		::SendMessage(GetDlgItem(HAL_ADDT_ALLOC_COMBO), CB_SETCURSEL , allocation_type_, 0);
		
		BOOL retval =  DoAllDataxchange(false);
		return retval;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return this->IsDialogMessage(pMsg);
	}
	
	void OnCancel(UINT, int, HWND hWnd)
	{
		GetWindowRect(rect_);
		EndDialog(IDCANCEL);
	}
	
	void OnOk(UINT, int, HWND hWnd)
	{
		DoAllDataxchange(true);

		allocation_type_ = allocation_type_map_[allocation_type_name_];

		GetWindowRect(rect_);
		EndDialog(IDOK);
	}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
	{
		using boost::serialization::make_nvp;
		switch (version)
		{
		case 2:			
			ar & boost::serialization::make_nvp("rect", rect_);
		
		break;

		case 1:
		default:
			assert(false);
		}
	}

private:
	BOOL DoAllDataxchange(bool direction)
	{
		adjustDlg_.DoDataExchange(direction);
		return DoDataExchange(direction);
	}

	HaliteSaveAndMoveToDlg adjustDlg_;

	WTL::CRect rect_;
	bool& startPaused_;
	bool& managed_;
	hal::bit::allocations& allocation_type_;
	
	wstring allocation_type_name_;
	wstring& name_or_uri_;
	bool can_edit_name_;
	wstr2alloc_map allocation_type_map_;
};

BOOST_CLASS_VERSION(AddTorrentDialog, 2)

#endif // RC_INVOKED
