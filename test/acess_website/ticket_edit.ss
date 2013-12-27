#!/usr/local/bin/sw
#<?php

include "common.inc.ss";

if( $gUserID == 0 )
{
	$tpl->Assign("ErrorMessage", "You must be logged in to modify a ticket");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String $lTicketID = CGI.ReadGET("id");
if($lTicketID === null)
{
	$tpl->Assign("ErrorMessage", "Please select a ticket");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String $lCommentText = CGI.ReadPOST("comment_text");
String $lAction = CGI.ReadPOST("action");

String $query = "SELECT ticket_status FROM tickets WHERE ticket_id="+(Integer)$lTicketID+" LIMIT 1";
String[] $row = DB_GetSingleRow($query);
if( $row === null ) {
	$tpl->Assign("ErrorMessage", "Invalid ticket ID");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String $oldStatus = $row[0];
String $newStatus = null;
String $lResolution = CGI.ReadGET("resolution");

switch($lAction)
{
case "none":
	// Do nothing
	break;
case "reopen":
	// Sanity - Check that status is 'closed'
	
	// Mark ticket as needswork
	$newStatus = "needswork";
	break;
case "close":
	// Sanity - Check that status is 'reviewing'
	$newStatus = "closed";
	break;
case "needswork":
	// Sanity - Check that status is 'reviewing'
	$newStatus = "needswork";
	break;
case "resolve":
	// Sanity - Check that status is 'new/accepted/needswork'
	if( $oldStatus != "new" && $oldStatus != "accepted" && $oldStatus != "needswork" )
	{
		$tpl->Assign("ErrorMessage", "Resolve used on invalid status ("+$oldStatus+")");
		$tpl->Display("templates/error.tpl");
		return 0;
	}
	$newStatus = "reviewing";
	break;
case "accept":
	// Sanity - Check that status is 'new'
	$newStatus = "accepted";
	break;
default:
	// Warning - Unknown action
	$tpl->Assign("ErrorMessage", "Unknown ticket action '"+$lAction+"'");
	$tpl->Display("templates/error.tpl");
	return 0;
}

Integer $commentNum = -1;

if( $lCommentText != "" || $newStatus != $oldStatus )
{
//	String $updateText = 
}
else
{
	// No change, just redirect back to the top
}

// Redirect back to ticket
$tpl->Assign("Target", "ticket_view.ss?id="+$lTicketID+"#comment"+$commentNum);
$tpl->Display("templates/redirect.tpl");

# vim: ft=php

