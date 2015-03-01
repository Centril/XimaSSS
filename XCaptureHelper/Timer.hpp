typedef void (*ThreadTimerProc)(void);

class CThreadTimer  
{  
    void* object;			// pointer to the "parent" object (like CTimersDlg)
    UINT idEvent;			// timer ID
    UINT elapse;			// "Sleep time" in milliseconds
    ThreadTimerProc proc;	// Callback function, supplied by the user

    CRITICAL_SECTION lock;  // thread synchronization

    static DWORD WINAPI ThreadFunction (LPVOID pParam); // thread entry point
public:
    BOOL isActive;			// Set to FALSE after the call to KillTimer
	CThreadTimer();
	virtual ~CThreadTimer();
    
    UINT SetTimer (void* obj, UINT nIDEvent, UINT uElapse, ThreadTimerProc lpTimerProc);
    BOOL KillTimer();
};