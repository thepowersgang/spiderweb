#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");
$tpl->Assign("PageTitle", "File Ticket");

if( $gUserID == 0 ) {
	$tpl->Display("templates/login_required.tpl");
	return 0;
}

String	$lTitle = CGI.ReadPOST("title");
String	$lDesc = CGI.ReadPOST("desc");
String	$lProject = CGI.ReadPOST("prj");
String	$lType = CGI.ReadPOST("type");

if( $lProject === null ) {
	$lProject = CGI.GetPathInfo(0) ?: CGI.ReadGET("prj");
	$lType = CGI.GetPathInfo(1) ?: CGI.ReadGET("type");
}

if( $lTitle !== null )
{
	String $q = "INSERT INTO tickets (ticket_title,ticket_filer,ticket_type,ticket_project,ticket_desc)"
		+" VALUES ("
		+"'"+$dbconn->Escape($lTitle)+"',"
		+$gUserID+","
		+"'"+$dbconn->Escape($lType)+"',"
		+"'"+$dbconn->Escape($lProject)+"',"
		+"'"+$dbconn->Escape($lDesc)+"'"
		+")";
	if( $dbconn->Query($q) === null ) {
		$tpl->Assign("SQLQuery", $q);
		$tpl->Assign("SQLError", $dbconn->LastErrorString());
		$tpl->Display("templates/sqlerror.tpl");
		return 0;
	}
	$tpl->Display("templates/ticket_filed.tpl");
	return 0;
}

$tpl->Assign("lProject", $lProject);
$tpl->Assign("lType", $lType);

$tpl->Display("templates/ticket_file.tpl");

# vim: ft=php
