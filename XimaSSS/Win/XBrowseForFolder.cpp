// XBrowseForFolder.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// Description:
//     XBrowseForFolder.cpp implements XBrowseForFolder(), a function that
//     wraps SHBrowseForFolder().
//
// History
//     Version 1.0 - 2003 September 25
//     - Initial public release
//
// This software is released into the public domain.  You are free to use it 
// in any way you like.
//
// This software is provided "as is" with no expressed or implied warranty.  
// I accept no liability for any damage or loss of business that this software 
// may cause.
//
///////////////////////////////////////////////////////////////////////////////

// if you don't want to use MFC, comment out the following line:
#include "../StdAfx.hpp"
#include "Dialog.class.hpp"
#pragma warning(disable: 4005)
#define _ASSERTE(x) assert(x)
#pragma warning(enable: 4005)

#include "Shlobj.h"
#include "io.h"
#include <tchar.h>
#include "XBrowseForFolder.hpp"

#define TRACE (void(0))

///////////////////////////////////////////////////////////////////////////////
// Rect - a minimal Rect class
class CRect : public tagRECT
{
public:
	CRect()
	{
	}

	CRect(const int &l, const int &t, const int &r, const int &b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	int width(void) const
	{
		return right - left;
	}

	int height(void) const
	{
		return bottom - top;
	}

	void swapLeftRight(void)
	{
		SwapLeftRight(LPRECT(this));
	}
	static void SwapLeftRight(RECT* lpRect)
	{
		long temp = lpRect->left;
		lpRect->left = lpRect->right;
		lpRect->right = temp;
	}

	operator RECT*()
	{
		return this;
	}
};

///////////////////////////////////////////////////////////////////////////////
// ScreenToClientX - helper function in case non-MFC
inline static void ScreenToClientX(Win::Window &_w, LPRECT lpRect)
{
	_w.screenToClient((POINT*)lpRect);
	_w.screenToClient(( (POINT*)lpRect )+1);
}

///////////////////////////////////////////////////////////////////////////////
// MoveWindowX - helper function in case non-MFC
inline static void MoveWindowX(Win::Window &_w, CRect& rect, const bool &repaint)
{
	_w.move(rect.left, rect.top, rect.width(), rect.height(), repaint);
}

///////////////////////////////////////////////////////////////////////////////
// BrowseCallbackProc - SHBrowseForFolder callback function
inline static int __stdcall BrowseCallbackProc(HWND hwnd, unsigned int uMsg, long _lParam, long lpData)
{
	Win::Dialog _w(hwnd);
	switch(uMsg)
	{
	case BFFM_INITIALIZED:		// sent when the browse dialog box has finished initializing. 
	{
		// remove context help button from dialog caption
		long lStyle = _w.getSetting(GWL_STYLE);
		lStyle &= ~DS_CONTEXTHELP;
		_w.setSetting(GWL_STYLE, lStyle);
		lStyle = _w.getSetting(GWL_EXSTYLE);
		lStyle &= ~WS_EX_CONTEXTHELP;
		_w.setSetting(GWL_EXSTYLE, lStyle);
		// set initial directory
		_w.sendMessage(BFFM_SETSELECTION, 1, lpData);

		try
		{
			// find the folder tree and make dialog larger
			Win::Window _tree(FindWindowEx(*_w, 0, _T("SysTreeView32"), 0));
			// make the dialog larger
			CRect rectDlg;
			_w.getRect(&rectDlg);
			rectDlg.right += 40;
			rectDlg.bottom += 30;
			MoveWindowX(_w, rectDlg, true);
			_w.getClientRect(&rectDlg);

			// move the Cancel button
			CRect rectCancel(0, 0, 0, 0);
			Win::Window _cancel = _w.getItem(IDCANCEL);
			_cancel.getRect(&rectCancel);
			ScreenToClientX(_w, &rectCancel);
			int h = rectCancel.height();
			int w = rectCancel.width();
			rectCancel.bottom = rectDlg.bottom - 5;
			rectCancel.top = rectCancel.bottom - h;
			rectCancel.right = rectDlg.right - 5;
			rectCancel.left = rectCancel.right - w;
			MoveWindowX(_cancel, rectCancel, 0);
			// move the OK button
			CRect rectOK(0, 0, 0, 0);
			Win::Window _ok = _w.getItem(IDOK);
			_ok.getRect(&rectOK);
			ScreenToClientX(_w, &rectOK);
			rectOK.bottom = rectDlg.bottom - 5;
			rectOK.top = rectOK.bottom - h;
			rectOK.right = rectCancel.left - 10;
			rectOK.left = rectOK.right - w;
			MoveWindowX(_ok, rectOK, 0);

			// expand the folder tree to fill the dialog
			CRect rectTree;
			_tree.getRect(&rectTree);
			ScreenToClientX(_w, &rectTree);
			rectTree.top = 5;
			rectTree.left= 5;
			rectTree.bottom = rectOK.top - 5;
			rectTree.right = rectDlg.right - 5;
			MoveWindowX(_tree, rectTree, 0);
		}
		catch(...)
		{
			return 0;
		}
		break;
	}
	case BFFM_SELCHANGED:		// sent when the selection has changed
	{
		TCHAR szDir[MAX_PATH*2] = { 0 };

		// fail if non-filesystem
		int bRet = SHGetPathFromIDList((LPITEMIDLIST) _lParam, szDir);
		if(bRet)
		{
			// fail if folder not accessible
			if(_taccess(szDir, 00) != 0)
			{
				bRet = FALSE;
			}
			else
			{
				SHFILEINFO sfi;
				::SHGetFileInfo((LPCTSTR)_lParam, 0, &sfi, sizeof(sfi), 
					SHGFI_PIDL | SHGFI_ATTRIBUTES);
				__noop TRACE(_T("dwAttributes=0x%08X\n"), sfi.dwAttributes);

				// fail if pidl is a link
				if (sfi.dwAttributes & SFGAO_LINK)
				{
					__noop TRACE(_T("SFGAO_LINK\n"));
					bRet = FALSE;
				}
			}
		}

		// if invalid selection, disable the OK button
		if(!bRet)
		{
			Window _ok = _w.getItem(IDOK);
			_ok.disable();
		}

		__noop TRACE(_T("szDir=%s\n"), szDir);
		break;
	}
	}
         
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// XBrowseForFolder()
//
// Purpose:     Invoke the SHBrowseForFolder API.  If lpszInitialFolder is
//              supplied, it will be the folder initially selected in the tree 
//              folder list.  Otherwise, the initial folder will be set to the 
//              current directory.  The selected folder will be returned in 
//              lpszBuf.
//
// Parameters:  hWnd              - handle to the owner window for the dialog
//              lpszInitialFolder - initial folder in tree;  if NULL, the initial
//                                  folder will be the current directory.
//              lpszBuf           - buffer for the returned folder path
//              dwBufSize         - size of lpszBuf in TCHARs
//
// Returns:     BOOL - TRUE = success;  FALSE = user hit Cancel
//
bool XBrowseForFolder(Win::Window &_w, LPCTSTR lpszInitialFolder, LPTSTR lpszBuf, const unsigned long &dwBufSize)
{
	_ASSERTE(lpszBuf);
	_ASSERTE(dwBufSize >= MAX_PATH);

	if(lpszBuf == 0 || dwBufSize < MAX_PATH)
		return false;

	lpszBuf[0] = _T('\0');

	TCHAR szInitialPath[MAX_PATH*2];
	memset(szInitialPath, 0, sizeof szInitialPath);

	if(lpszInitialFolder && lpszInitialFolder[0] != _T('\0'))
		_tcsncpy(szInitialPath, lpszInitialFolder, sizeof(szInitialPath)/sizeof(TCHAR)-2);
	else
		::GetCurrentDirectory(sizeof(szInitialPath)/sizeof(TCHAR)-2, szInitialPath);

	BROWSEINFO bi;
	memset(&bi, 0, sizeof BROWSEINFO);
	bi.hwndOwner = *_w;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;	// do NOT use BIF_NEWDIALOGSTYLE, // BIF_EDITBOX, or BIF_STATUSTEXT
	bi.lpfn      = BrowseCallbackProc;
	bi.lParam    = (long) szInitialPath;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	int bRet = 0;
	if(pidl)
	{
		TCHAR szBuffer[MAX_PATH*2];
		szBuffer[0] = _T('\0');

		if(SHGetPathFromIDList(pidl, szBuffer))
		{
			memset(lpszBuf, 0, dwBufSize);
			_tcsncpy(lpszBuf, szBuffer, dwBufSize - 1);
			bRet = 1;
		}
		else
			__noop TRACE(_T("SHGetPathFromIDList failed\n"));

		IMalloc *pMalloc = 0; 
		if(SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) 
		{  
			pMalloc->Free(pidl);  
			pMalloc->Release(); 
		}
	}

	return bRet != 0 ? true : false;
}
