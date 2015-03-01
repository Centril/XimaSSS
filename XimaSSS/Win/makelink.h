#ifndef _MAKELINK_H
#define _MAKELINK_H

#ifdef __cplusplus
extern "C"
{
#endif
	int CreateShortCut(char* pszTargetfile, char* pszTargetargs, char* pszLinkfile, char* pszDescription,
					int iShowmode, char* pszCurdir, char* pszIconfile, int iIconindex);
#ifdef __cplusplus
}
#endif
#endif