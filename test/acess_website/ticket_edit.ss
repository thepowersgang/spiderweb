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

if( $lAction == "none" )
	// Do nothing :)
elseif( $lAction == "reopen" )
{
	// Sanity - Check that status is 'closed'
	
	// Mark ticket as needswork
}
elseif( $lAction == "close" )
{
	// Sanity - Check that status is 'reviewing'
}

# vim: ft=php

