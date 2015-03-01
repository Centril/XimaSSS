#pragma once

// Capture constants
#define basemsg 1000

static const int OpenGUI = basemsg + 1;
static const int FTPFiles = basemsg + 2;
static const int Exit = basemsg + 3;

#define captmsg 2000
static const int MultiMonitor = captmsg + 1;
static const int PrimaryScreen  = captmsg + 2;
static const int CaptureWindow  = captmsg + 3;
static const int Foreground  = captmsg + 4;
static const int Area  = captmsg + 5;
static const int WorkingArea  = captmsg + 6;

#define captrecallmsg captmsg + 1000
static const int CW_WindowSelected = captrecallmsg + 1;
static const int Area_AreaSelected = captrecallmsg + 2;
static const int CW_Abort = captrecallmsg + 3;
static const int Area_Abort = captrecallmsg + 4;
static const int CW_GetIsBlinking = captrecallmsg + 5;
// GUI constants
#define GUI_SAVE WM_USER + 1