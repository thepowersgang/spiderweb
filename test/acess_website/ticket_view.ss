#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");

String $lTicketID_str = CGI.ReadGET("id");
if($lTicketID_str === null)
{
	$tpl->Assign("ErrorMessage", "Please select a ticket");
	$tpl->Display("templates/error.tpl");
	return 0;
}
Integer $lTicketID = (Integer)$lTicketID_str;

String $query = "SELECT ticket_title,ticket_desc,ticket_type,ticket_project,ticket_tags"
		+",ticket_status,ticket_opened"
		+",ticket_owner,ticket_filer"
		+" FROM tickets WHERE ticket_id="+$lTicketID+" LIMIT 1";
String[] $row = DB_GetSingleRow($tpl, $dbconn, $query);
if( $row === null ) {
	$tpl->Assign("ErrorMessage", "Invalid ticket ID");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String tickets_userlink(String $uid)
{
	return "<a href='users.ss?id="+$uid+"'>User #"+$uid+"</a>";
}

$tpl->Assign("PageTitle", "View Ticket");

$tpl->Assign("lTicketID", $lTicketID);
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
//$lTicket->set("owner",   $row[7]);
//$lTicket->set("filer",   $row[8]);
$tpl->Assign("lTicket", $lTicket);

// TODO: Comments
SpiderWeb.MySQL.Result $res = DB_Query($tpl, $dbconn,
	"SELECT cmt_time,cmt_user,cmt_text FROM ticket_comments WHERE cmt_ticket="+$lTicketID
	);
Lang.StringMap[] $lComments($res->ResultCount());
//String[] $row;
Integer $i = 0;
while( ($row = $res->GetNextRow()) !== null )
{
	Lang.StringMap $comment();
	$comment->set("time", $row[0]);
	$comment->set("user", Ticket_GetUser($dbconn, (Integer)$row[1]));
	$comment->set("num", (String)$i);
	$comment->set("text", $row[2]);
	$lComments[$i++] = $comment;
}
$tpl->Assign("lComments", $lComments);

$tpl->Display("templates/ticket_view.tpl");

# vim: ft=php
