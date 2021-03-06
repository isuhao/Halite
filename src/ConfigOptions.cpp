
//         Copyright Eóin O'Callaghan 2006 - 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "stdAfx.hpp"
#include "Halite.hpp"

#include "ConfigOptions.hpp"
#include "ProgressDialog.hpp"

void SecurityOptions::onFilterImport(UINT, int, HWND hWnd)
{
	CSSFileDialog dlgOpen(TRUE, NULL, NULL, OFN_HIDEREADONLY, L"eMule ipfilter.dat. (*.dat)|*.dat|", m_hWnd);
	
	if (dlgOpen.DoModal() == IDOK) 
	{
		ProgressDialog progDlg(L"Importing IP filters...", boost::bind(
			&hal::bit::ip_filter_import_dat, &hal::bittorrent(), path(hal::to_utf8(dlgOpen.m_ofn.lpstrFile)), _1, true));
		progDlg.DoModal();
	}
}	

BOOL GeneralOptions::OnInitDialog(HWND hwndFocus, LPARAM lParam)
{
#	if (_ATL_VER > 0x0700)
	lang_list_.Attach(GetDlgItem(HAL_LANGLIST));
	lang_list_.AddString(L"English");
	lang_list_.SetCurSel(0);

	if (hal::fs::is_directory(hal::app().exe_path().branch_path()))
	{
		hal::fs::directory_iterator end_iter;
		
		for (hal::fs::directory_iterator dir_itr(hal::app().exe_path().branch_path());
				dir_itr != end_iter; ++dir_itr )
		{
			if (hal::fs::is_regular(dir_itr->status()))
			{
				hal::xp::wsregex rex = hal::xp::wsregex::compile(L"[\\s\\w\\(\\)_-]+\\.[dD][lL]{2}");
				hal::xp::wsmatch what;

				if(hal::xp::regex_match(dir_itr->path().filename().wstring(), what, rex))
				{
					HMODULE hMod = ::LoadLibraryEx(dir_itr->path().wstring().c_str(), 0, LOAD_LIBRARY_AS_DATAFILE);

					// The upper size limit ain't nice, but at least it's safe from buffer overflow
					const int buffer_size = 512;
					boost::array<wchar_t, buffer_size> buffer;
					int length = ::LoadString(hMod, HALITE_LANGUAGE, buffer.elems, buffer_size);
					
					if (length)
					{
						wstring lang_name(buffer.elems);
						lang_map_[lang_name] = dir_itr->path().filename();
						int index = lang_list_.AddString(lang_name.c_str());
						
						if (dir_itr->path().filename() == halite().dll_) 
							lang_list_.SetCurSel(index);
					}
					::FreeLibrary(hMod);
				}
			}
		}
	}
#	endif // (_ATL_VER < 0x0700)	

	autosizeClass::CtrlsArrange();
	BOOL retval = DoDataExchange(false);

	onCompleteBalloonCheck(0, 0, GetDlgItem(HAL_GENERAL_COMPLETE_BALLOON));

	return retval;
}

int GeneralOptions::OnApply()
{	
	hal::event_log().post(shared_ptr<hal::EventDetail>(
		new hal::EventMsg(L"Applying general options.", hal::event_logger::info)));

#	if (_ATL_VER > 0x0700)
	size_t length = lang_list_.GetTextLen(lang_list_.GetCurSel());
	boost::scoped_array<wchar_t> buffer(new wchar_t[length+1]);
	
	lang_list_.GetText(lang_list_.GetCurSel(), buffer.get());
	std::wstring language(buffer.get(), length);
	if (language == L"English")
	{
		halite().dll_.clear();
		hal::app().res_revert();
	}
	else
	{
		halite().dll_ = lang_map_[language].native();
		hal::app().res_set_dll(lang_map_[language].native());
	}
#	endif // (_ATL_VER < 0x0700)		
	return DoDataExchange(true);
}
