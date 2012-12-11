#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");
$tpl->Assign("PageTitle", "Issue List");

String $query = "SELECT ticket_id,ticket_title,ticket_filer,ticket_type,ticket_status,ticket_opened,ticket_owner FROM tickets"
	+" WHERE 1=1";
String $lProject = CGI.ReadGET("prj");
String $lType = CGI.ReadGET("type");
if( $lProject )
	$query += " AND ticket_project='" + $dbconn->Escape(CGI.ReadGET("prj")) + "'";
if( $lType )
	$query += " AND ticket_type='" + $dbconn->Escape(CGI.ReadGET("type")) + "'";
$tpl->Assign("lTypeCodes", "prj="+$lProject+"&amp;type="+$lType+"&amp;");
$tpl->Assign("lProject", $lProject);
if( $lType )
	$tpl->Assign("lType", $lType);
else
	$tpl->Assign("lType", "");

String $lSort = CGI.ReadGET("sort");
if( $lSort !== null )
{
	if($lSort == "id")
		$query += " ORDER BY `ticket_id` ASC";
	else if($lSort == "title")
		$query += " ORDER BY `ticket_title` ASC";
	else if($lSort == "filed")
		$query += " ORDER BY `ticket_opened` ASC";
	else {
		$lSort = "id";
		$query += " ORDER BY `ticket_id` ASC";
	}
}
$tpl->Assign("lSort", $lSort);

SpiderWeb.MySQL.Result $res = $dbconn->Query($query);
if( $res === null ) {
	$tpl->Assign("SQLQuery", $query);
	$tpl->Assign("SQLError", $dbconn->LastErrorString());
	$tpl->Display("templates/sqlerror.tpl");
	return 0;
}

Lang.StringMap[]	$tickets($res->ResultCount());
String[]	$row;

for(Integer $i = 0; ($row = $res->GetNextRow()) !== null; $i++)
{
	Lang.StringMap	$ticket();

	$ticket->set("ID", $row[0]);
	$ticket->set("title", $row[1]);
	$ticket->set("filer", Ticket_GetUser($dbconn, (Integer)$row[2]));	// TODO: Get username?
	$ticket->set("type",   $row[3]);
	$ticket->set("status", $row[4]);
	$ticket->set("opened", $row[5]);
	$ticket->set("owner", Ticket_GetUser($dbconn, (Integer)$row[6]));

	$tickets[$i] = $ticket;
}

$tpl->Assign("lTicketList", $tickets);
$tpl->Display("templates/ticket_list.tpl");

# vim: ft=php
