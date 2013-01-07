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
String[] $row = DB_GetSingleRow($tpl, $dbconn, $query);
if( $row === null ) {
	$tpl->Assign("ErrorMessage", "Invalid ticket ID");
	$tpl->Display("templates/error.tpl");
	return 0;
}

String $oldStatus = $row[0];
String $newStatus = null;
String $lResolution = CGI.ReadGET("resolution");

if( $lAction == "none" )
	// Do nothing :)
elseif( $lAction == "reopen" )
{
	// Sanity - Check that status is 'closed'
	
	// Mark ticket as needswork
	$newStatus = "needswork";
}
elseif( $lAction == "close" )
{
	// Sanity - Check that status is 'reviewing'
	$newStatus = "closed";
}
elseif( $lAction == "needswork" )
{
	// Sanity - Check that status is 'reviewing'
	$newStatus = "needswork";
}
elseif( $lAction == "resolve" )
{
	// Sanity - Check that status is 'new/accepted/needswork'
	if( $oldStatus != "new" && $oldStatus != "accepte" && $oldStatus != "needswork" )
	{
		$tpl->Assign("ErrorMessage", "Resolve used on invalid status ("+$oldStatus+")");
		$tpl->Display("templates/error.tpl");
		return 0;
	}
	$newStatus = "reviewing";
}
elseif( $lAction == "accept" )
{
	// Sanity - Check that status is 'new'
	$newStatus = "accepted";
}
else
{
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

