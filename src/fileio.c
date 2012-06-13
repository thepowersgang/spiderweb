/*
 * SpiderWeb
 * - By John Hodge (thePowersGang)
 * 
 * File IO Namespace / Object
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spiderscript.h>
#include "spiderweb_internal.h"

// === TYPES ===
typedef struct {
	FILE	*FP;
} t_obj_IO_File;

// === PROTOTYPES ===
FCN_PROTO(IO_File__construct);
void	IO_File__destruct(tSpiderObject *This);

// === GLOBALS ===
DEF_OBJ_FCN(IO_File_Write, "Write", NULL,          SS_DATATYPE_INTEGER, -2, SS_DATATYPE_STRING, 0);
DEF_OBJ_FCN(IO_File_Seek,  "Seek",  IO_File_Write, SS_DATATYPE_INTEGER, -2,SS_DATATYPE_INTEGER, SS_DATATYPE_INTEGER, 0);
DEF_OBJ_FCN(IO_File_Read,  "Read",  IO_File_Seek,  SS_DATATYPE_STRING,  -2,SS_DATATYPE_INTEGER, 0);
tSpiderFunction	g_obj_IO_File__construct = {NULL,"",IO_File__construct,0,{SS_DATATYPE_STRING,SS_DATATYPE_STRING,0}};
tSpiderClass	g_obj_IO_File = {
	NULL, "IO@File",
	&g_obj_IO_File__construct,	// Constructor - Open File
	IO_File__destruct,
	&g_fcn_IO_File_Read,	// First function
	1,
	{
		{"offset", SS_DATATYPE_INTEGER, 0, 0},
	}
};

// === CODE ===
FCN_PROTO(IO_File__construct)
{
	const tSpiderString	*filename, *mode;
	tSpiderObject	*ret;
	FILE	*fp;

	if( NArgs != 2 )
		return -1;
	if( ArgTypes[0] != SS_DATATYPE_STRING || ArgTypes[1] != SS_DATATYPE_STRING )
		return -1;

	filename = Args[0];
	mode = Args[1];

//	printf("fopen('%.*s', '%.*s')\n", (int)filename->Length, filename->Data, (int)mode->Length, mode->Data);
	// TODO: Better validation of input data
	// TODO: What if the strings aren't NULL terminated?
	fp = fopen(filename->Data, mode->Data);
	if( !fp ) {
//		perror("Opening for script");
		*(tSpiderObject**)RetData = NULL;
		return 0;
	}
	
	ret = SpiderScript_AllocateObject(Script, &g_obj_IO_File, sizeof(t_obj_IO_File));
	
	((t_obj_IO_File*)ret->OpaqueData)->FP = fp;
	*(tSpiderInteger*)ret->Attributes[0] = 0;
	
	*(tSpiderObject**)RetData = ret;
	return 0;
}
void IO_File__destruct(tSpiderObject *This)
{
	t_obj_IO_File	*info = This->OpaqueData;
	fclose( info->FP );
}

FCN_PROTO(IO_File_Read)
{
	t_obj_IO_File	*info;
	const tSpiderObject	*this;
	const tSpiderInteger	*val_size;
	tSpiderString	*ret;
	
	if( NArgs != 2 )
		return -1;
	if( ArgTypes[1] != SS_DATATYPE_INTEGER )
		return -1;
	
	this = Args[0];
	val_size = Args[1];
	info = this->OpaqueData;
	
	ret = SpiderScript_CreateString(*val_size, NULL);
	ret->Length = fread(ret->Data, 1, *val_size, info->FP);
	*(tSpiderString**)RetData = ret;
	
	return SS_DATATYPE_STRING;
}

FCN_PROTO(IO_File_Seek)
{
	t_obj_IO_File	*info;
	const tSpiderObject	*this;
	const tSpiderInteger	*val_offset, *val_dir;
	 int	dir;
	
	if( NArgs != 3 )	return -1;
	this = Args[0];
	info = this->OpaqueData;
	val_offset = Args[1];
	val_dir = Args[2];
	
	if(*val_dir < 0)	dir = SEEK_END;
	if(*val_dir > 0)	dir = SEEK_SET;
	if(*val_dir == 0)	dir = SEEK_CUR;
	
	fseek(info->FP, *val_offset, dir);

	*(tSpiderInteger*)RetData = ftell(info->FP);
	return SS_DATATYPE_INTEGER;
}

FCN_PROTO(IO_File_Write)
{
	t_obj_IO_File	*info;
	const tSpiderObject	*this;
	const tSpiderString	*val_data;
	 int	ret_val;
	
	if( NArgs != 2 ) {
		fprintf(stderr, "IO@File.Write - NArgs bad %i != 2\n", NArgs);
		return -1;
	}
	if( !Args[1] || ArgTypes[1] != SS_DATATYPE_STRING ) {
		fprintf(stderr, "IO@File.Write - Args[1] 0x%x %p not SS_DATATYPE_STRING",
			ArgTypes[1], Args[1]);
		return -1;
	}
	
	this = Args[0];
	info = this->OpaqueData;
	val_data = Args[1];
	
	ret_val = fwrite(val_data->Data, 1, val_data->Length, info->FP);
	*(tSpiderInteger*)RetData = ret_val;
	
	return SS_DATATYPE_INTEGER;
}

