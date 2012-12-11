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

$tpl->Assign("PageTitle", "View Ticket");

Lang.StringMap	$lTicket();
$lTicket->set("title",   $row[0]);
$lTicket->set("desc",    formatText($row[1]));
$lTicket->set("type",    $row[2]);
$lTicket->set("project", $row[3]);
// TODO: Tags
$lTicket->set("status",  $row[5]);
$lTicket->set("opened",  $row[6]);
$lTicket->set("owner",   Ticket_GetUser($dbconn, (Integer)$row[7]));
$lTicket->set("filer",   Ticket_GetUser($dbconn, (Integer)$row[8]));
$tpl->Assign("lTicket", $lTicket);

$tpl->Display("templates/ticket_view.tpl");

# vim: ft=php
