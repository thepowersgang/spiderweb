#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");

String $lTicketID = CGI.ReadGET("id");
if($lTicketID === null)
{
	$tpl->Assign("ErrorMessage", "Please select a ticket");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String $query = "SELECT ticket_title,ticket_desc,ticket_type,ticket_project,ticket_tags"
		+",ticket_status,ticket_opened"
		+",ticket_owner,ticket_filer"
		+" FROM tickets WHERE ticket_id="+(Integer)$lTicketID+" LIMIT 1";
SpiderWeb.MySQL.Result $res = $dbconn->Query($query);
if( $res === null ) {
	$tpl->Assign("SQLQuery", $query);
	$tpl->Assign("SQLError", $dbconn->LastErrorString());
	$tpl->Display("templates/sqlerror.tpl");
	return 0;
}
String[] $row = $res->GetNextRow();
if( $row === null ) {
	$tpl->Assign("ErrorMessage", "Invalid ticket ID");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String	$lTicketTitle   = $row[0];
String	$lTicketDesc    = formatText($row[1]);
String	$lTicketType    = $row[2];
String	$lTicketProject = $row[3];
String	$lTicketStatus  = $row[5];

$tpl->Assign("PageTitle", "View Ticket");

Lang.StringMap	$lTicket();
$lTicket->set("title",   $lTicketTitle);
$lTicket->set("desc",    $lTicketDesc);
$lTicket->set("type",    $lTicketType);
$lTicket->set("project", $lTicketProject);
$lTicket->set("status", $lTicketStatus);
$tpl->Assign("lTicket", $lTicket);

$tpl->Display("templates/ticket_view.tpl");

# vim: ft=php
