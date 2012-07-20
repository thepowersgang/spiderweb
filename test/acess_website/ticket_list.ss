#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");
$tpl->Assign("PageTitle", "Issue List");

String $query = "SELECT ticket_id,ticket_title,ticket_filer,ticket_type,ticket_status FROM tickets"
	+" WHERE 1=1";
if( CGI.ReadGET("prj") )
	$query += " AND ticket_project='" + $dbconn->Escape(CGI.ReadGET("prj")) + "'";
if( CGI.ReadGET("type") )
	$query += " AND ticket_type='" + $dbconn->Escape(CGI.ReadGET("type")) + "'";

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

	$tickets[$i] = $ticket;
}

$tpl->Assign("lTicketList", $tickets);
$tpl->Display("templates/ticket_list.tpl");

#vim: ft=php
