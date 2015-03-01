/*
--------------------------------------------------------------------------------
File Name   : MAKELINK

Contains    : Console mode program (EXE file) to create a link file (short-cut).
              This is based on:
              
              (i) An example in MSDN: Platform SDK -> User Interface Services ->
              Shell and Common Controls -> Windows Shell API -> Shell links.
              Microsoft keep moving the articles in MSDN so it may be easier 
              to search for the example by its name (CreateLink).

              (ii) A public domain program on the web that uses the same
              technique as MSDN: http://www.metathink.com/shlink/shlink.c

Legal       : Copyright (c) 2005. All rights reserved.
--------------------------------------------------------------------------------
*/

/* Windows headers come first */
#define STRICT
/* COM headers (requires shell32.lib, ole32.lib, uuid.lib) */
#include <objbase.h>
#include <shlobj.h>

#include "makelink.h"
/*
--------------------------------------------------------------------------------
Description:
  Creates the actual 'lnk' file (assumes COM has been initialized).

Parameters:
  pszTargetfile    - File name of the link's target, must be a non-empty
                     string.

  pszTargetargs    - Command line arguments passed to link's target, may
                     be an empty string.

  pszLinkfile      - File name of the actual link file, must be a non-empty
                     string.

  pszDescription   - Description of the linked item. If this is an empty
                     string the description is not set.

  iShowmode        - ShowWindow() constant for the link's target. Use one of:
                       1 (SW_SHOWNORMAL) = Normal window.
                       3 (SW_SHOWMAXIMIZED) = Maximized.
                       7 (SW_SHOWMINNOACTIVE) = Minimized.
                     If this is zero the showmode is not set.

  pszCurdir        - Working directory of the active link. If this is
                     an empty string the directory is not set.

  pszIconfile      - File name of the icon file used for the link.
                     If this is an empty string the icon is not set.

  iIconindex       - Index of the icon in the icon file. If this is
                     < 0 the icon is not set.

Returns:
  HRESULT value >= 0 for success, < 0 for failure.
--------------------------------------------------------------------------------
*/
int CreateShortCut(char* pszTargetfile, char* pszTargetargs, char* pszLinkfile, char* pszDescription,
					int iShowmode, char* pszCurdir, char* pszIconfile, int iIconindex)
{
	long			hRes;                  /* Returned COM result code */
	IShellLink*		pShellLink;            /* IShellLink object pointer */
	IPersistFile*	pPersistFile;          /* IPersistFile object pointer */
	unsigned short	wszLinkfile[MAX_PATH]; /* pszLinkfile as Unicode string */
	int				iWideCharsWritten;     /* Number of wide characters written */

	if(FAILED(CoInitialize(0)))
		return 0;

	hRes = E_INVALIDARG;
	if
	(
		(pszTargetfile != 0) && (strlen(pszTargetfile) > 0) &&
		(pszTargetargs != 0) &&
		(pszLinkfile != 0) && (strlen(pszLinkfile) > 0) &&
		(pszDescription != 0) && 
		(iShowmode >= 0) &&
		(pszCurdir != 0) && 
		(pszIconfile != 0) &&
		(iIconindex >= 0)
	)
	{
		hRes = CoCreateInstance(&CLSID_ShellLink,	/* pre-defined CLSID of the IShellLink object */
                            NULL,					/* pointer to parent interface if part of aggregate */
                            CLSCTX_INPROC_SERVER,	/* caller and called code are in same process */
                            &IID_IShellLink,		/* pre-defined interface of the IShellLink object */
                            &pShellLink);			/* Returns a pointer to the IShellLink object */
		if(SUCCEEDED(hRes))
		{
			/* Set the fields in the IShellLink object */
			hRes = pShellLink->lpVtbl->SetPath(pShellLink, pszTargetfile);
			hRes = pShellLink->lpVtbl->SetArguments(pShellLink, pszTargetargs);
			if(strlen(pszDescription) > 0)
				hRes = pShellLink->lpVtbl->SetDescription(pShellLink, pszDescription);
			if(iShowmode > 0)
				hRes = pShellLink->lpVtbl->SetShowCmd(pShellLink, iShowmode);
			if(strlen(pszCurdir) > 0)
				hRes = pShellLink->lpVtbl->SetWorkingDirectory(pShellLink, pszCurdir);
			if(strlen(pszIconfile) > 0 && iIconindex >= 0)
				hRes = pShellLink->lpVtbl->SetIconLocation(pShellLink, pszIconfile, iIconindex);

			/* Use the IPersistFile object to save the shell link */
			hRes = pShellLink->lpVtbl->QueryInterface(pShellLink,        /* existing IShellLink object */
														&IID_IPersistFile, /* pre-defined interface of the IPersistFile object */
														&pPersistFile);    /* returns a pointer to the IPersistFile object */
			if(SUCCEEDED(hRes))
			{
				iWideCharsWritten = MultiByteToWideChar(CP_ACP, 0, pszLinkfile, -1, wszLinkfile, MAX_PATH);
				hRes = pPersistFile->lpVtbl->Save(pPersistFile, wszLinkfile, TRUE);
				pPersistFile->lpVtbl->Release(pPersistFile);
			}
			pShellLink->lpVtbl->Release(pShellLink);
		}
	}
	CoUninitialize();
	return SUCCEEDED(hRes);
}