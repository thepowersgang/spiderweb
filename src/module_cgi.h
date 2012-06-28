
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

extern void	CGI_ParseGETData(void);

#endif

