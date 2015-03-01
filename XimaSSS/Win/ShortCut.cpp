#include <windows.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <objbase.h>
#include <shlobj.h>

bool CreateShortCut(char* pszTargetfile, char* pszTargetargs, char* pszLinkfile, char* pszDescription, const int &iShowmode,
					char* pszCurdir, char* pszIconfile, const int &iIconindex)
{
	if(CoInitialize(0) != S_OK)
		return false;

	HRESULT			hRes;                  /* Returned COM result code */
	IShellLink*		pShellLink;            /* IShellLink object pointer */
	IPersistFile*	pPersistFile;          /* IPersistFile object pointer */
	unsigned short	wszLinkfile[MAX_PATH]; /* pszLinkfile as Unicode string */
	int				iWideCharsWritten;     /* Number of wide characters written */

	hRes = E_INVALIDARG;
	if(
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
    hRes = CoCreateInstance(&CLSID_ShellLink,     /* pre-defined CLSID of the IShellLink object */
                            0,                 /* pointer to parent interface if part of aggregate */
                            CLSCTX_INPROC_SERVER, /* caller and called code are in same process */
                            &IID_IShellLink,      /* pre-defined interface of the IShellLink object */
                            &pShellLink);         /* Returns a pointer to the IShellLink object */
    if (SUCCEEDED(hRes))
    {
      /* Set the fields in the IShellLink object */
      hRes = pShellLink->lpVtbl->SetPath(pShellLink, pszTargetfile);
      hRes = pShellLink->lpVtbl->SetArguments(pShellLink, pszTargetargs);
      if (strlen(pszDescription) > 0)
      {
        hRes = pShellLink->lpVtbl->SetDescription(pShellLink, pszDescription);
      }
      if (iShowmode > 0)
      {
        hRes = pShellLink->lpVtbl->SetShowCmd(pShellLink, iShowmode);
      }
      if (strlen(pszCurdir) > 0)
      {
        hRes = pShellLink->lpVtbl->SetWorkingDirectory(pShellLink, pszCurdir);
      }
      if (strlen(pszIconfile) > 0 && iIconindex >= 0)
      {
        hRes = pShellLink->lpVtbl->SetIconLocation(pShellLink, pszIconfile, iIconindex);
      }

      /* Use the IPersistFile object to save the shell link */
      hRes = pShellLink->lpVtbl->QueryInterface(pShellLink,        /* existing IShellLink object */
                                                &IID_IPersistFile, /* pre-defined interface of the IPersistFile object */
                                                &pPersistFile);    /* returns a pointer to the IPersistFile object */
      if (SUCCEEDED(hRes))
      {
        iWideCharsWritten = MultiByteToWideChar(CP_ACP, 0, pszLinkfile, -1, wszLinkfile, MAX_PATH);
        hRes = pPersistFile->lpVtbl->Save(pPersistFile, wszLinkfile, TRUE);
        pPersistFile->lpVtbl->Release(pPersistFile);
      }
      pShellLink->lpVtbl->Release(pShellLink);
    }

  }
  CoUninitialize();
  return (hRes);
}