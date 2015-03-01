// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XMOUSEHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XMOUSEHOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef XCAPTUREHELPER_EXPORTS
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif

API bool __stdcall BeginWindowSelection(void);
API HWND __stdcall EndWindowSelection(void);

API bool __stdcall BeginAreaSelection(void);
API bool __stdcall EndAreaSelection(RECT& _rect);

void Test(string str)
{
	MessageBox(0, str.c_str(), "Note", MB_OK);
}