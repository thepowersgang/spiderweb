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
tSpiderObject	*IO_File__construct(tSpiderScript *Script, int NArgs, tSpiderValue **Args);
void	IO_File__destruct(tSpiderObject *This);

// === GLOBALS ===
DEF_OBJ_FCN(IO_File_Write, "Write", NULL, SS_DATATYPE_INTEGER, SS_DATATYPE_STRING, 0);
DEF_OBJ_FCN(IO_File_Seek, "Seek", IO_File_Write, SS_DATATYPE_INTEGER, SS_DATATYPE_INTEGER, SS_DATATYPE_INTEGER, 0);
DEF_OBJ_FCN(IO_File_Read, "Read", IO_File_Seek, SS_DATATYPE_STRING, SS_DATATYPE_INTEGER, 0);
tSpiderClass	g_obj_IO_File = {
	NULL, "IO@File",
	IO_File__construct,	// Constructor - Open File
	IO_File__destruct,
	NULL,	// Methods
	&g_fcn_IO_File_Read,	// First function
	1,
	{
		{"offset", SS_DATATYPE_INTEGER, 0, 0},
	}
};

// === CODE ===
tSpiderObject *IO_File__construct(tSpiderScript *Script, int NArgs, tSpiderValue **Args)
{
	tSpiderObject	*ret;
	FILE	*fp;

	if( NArgs != 2 )
		return ERRPTR;
	if( Args[0]->Type != SS_DATATYPE_STRING || Args[1]->Type != SS_DATATYPE_STRING )
		return ERRPTR;
	
	// TODO: Better validation of input data
	fp = fopen(Args[0]->String.Data, Args[1]->String.Data);
	if( !fp )	return NULL;
	
	ret = SpiderScript_AllocateObject(Script, &g_obj_IO_File, sizeof(t_obj_IO_File));
	
	((t_obj_IO_File*)ret->OpaqueData)->FP = fp;
	ret->Attributes[0] = SpiderScript_CreateInteger(0);
	
	return ret;
}
void IO_File__destruct(tSpiderObject *This)
{
	t_obj_IO_File	*info = This->OpaqueData;
	fclose( info->FP );
}

tSpiderValue *IO_File_Read(tSpiderScript *Script, int nParams, tSpiderValue **Parameters)
{
	t_obj_IO_File	*info;
	tSpiderObject	*this;
	tSpiderValue	*val_size, *ret;
	
//	printf("IO_File_Read: (%p, %i, %p)\n", Script, nParams, Parameters);
	
	if( nParams != 2 )	return ERRPTR;
	
	//TODO: Should 'this' be checked?
	
	if( !Parameters[1] )
		return NULL;
	
	val_size = SpiderScript_CastValueTo(SS_DATATYPE_INTEGER, Parameters[1]);
	
	this = Parameters[0]->Object;
	info = this->OpaqueData;
	
	ret = SpiderScript_CreateString(val_size->Integer, NULL);
	
//	printf("Reading %li from %p\n", val_size->Integer, info->FP);
	
	ret->String.Length = fread(ret->String.Data, 1, val_size->Integer, info->FP);
	
	SpiderScript_FreeValue(val_size);
	
	return ret;
}

tSpiderValue *IO_File_Seek(tSpiderScript *Script, int nParams, tSpiderValue **Parameters)
{
	t_obj_IO_File	*info;
	tSpiderObject	*this;
	tSpiderValue	*val_offset, *val_dir;
	 int	dir;
	
	if( nParams != 3 )	return ERRPTR;
	this = Parameters[0]->Object;
	info = this->OpaqueData;
	
	if( !Parameters[1] || !Parameters[2] )
		return NULL;
	
	val_offset = SpiderScript_CastValueTo(SS_DATATYPE_INTEGER, Parameters[1]);
	val_dir = SpiderScript_CastValueTo(SS_DATATYPE_INTEGER, Parameters[2]);
	
	if(val_dir->Integer < 0)	dir = SEEK_END;
	if(val_dir->Integer > 0)	dir = SEEK_SET;
	if(val_dir->Integer == 0)	dir = SEEK_CUR;
	
	fseek(info->FP, val_offset->Integer, dir);
	
	return SpiderScript_CreateInteger( ftell(info->FP) );
}
tSpiderValue *IO_File_Write(tSpiderScript *Script, int nParams, tSpiderValue **Parameters)
{
	t_obj_IO_File	*info;
	tSpiderObject	*this;
	tSpiderValue	*val_data;
	 int	ret_val;
	
	if( nParams != 2 )	return ERRPTR;
	
	if( !Parameters[1] )
		return NULL;
	
	val_data = SpiderScript_CastValueTo(SS_DATATYPE_STRING, Parameters[1]);
	
	this = Parameters[0]->Object;
	info = this->OpaqueData;
	
	ret_val = fwrite(val_data->String.Data, 1, val_data->String.Length, info->FP);
	
	return SpiderScript_CreateInteger(ret_val);
}
