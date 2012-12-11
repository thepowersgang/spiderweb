
#ifndef _CGI_H_
#define _CGI_H_

typedef struct sCGI_Param
{
	const char	*Name;
	 int	DataLen;
	const char	*Data;
} tCGI_Param;

extern int	giCGI_GETNArgs;
extern tCGI_Param	*gaCGI_GETArgs;
extern int	giCGI_POSTNArgs;
extern tCGI_Param	*gaCGI_POSTArgs;
extern int	giCGI_NCookies;
extern tCGI_Param	*gaCGI_Cookies;

extern int	CGI_int_URLEncode(const char *Source, int SourceLen, char *Dest);
extern void	CGI_ParseGETData(void);
extern void	CGI_ParsePOSTData(void);
extern void	CGI_ParseCookies(void);
extern void	CGI_AddHeader(const char *Format, ...);
extern void	CGI_SendHeadersOnce(void);

#endif

