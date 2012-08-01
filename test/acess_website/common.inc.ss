#<?php

include "database.inc.ss";

String $gSessionHash = CGI.GetCookie("SWSESSIONID");
CGI.Session $gSession($gSessionHash, "acess2sw");
//if($gSessionHash != $gSession->Hash) {
//	$gSessionHash = $gSession->Hash;
//	CGI.SetCookie("SWSESSIONID", $gSessionHash, 3600);
//}
String $gUserName = $gSession->Get("username");
Integer $gUserID = (Integer)$gSession->Get("uid");

if( $gUserName === null )
	$gUserName = "";

SpiderWeb.Template $tpl();
$tpl->Assign("UserName", $gUserName);


String formatText(String $Input)
{
	String $output = $Input;
	
#	$output = Lang.Strings.RegexReplace($output, "^.*^", "<sup>$1</sup>");
#	$output = Lang.Strings.RegexReplace($output, "_.*_", "<sub>$1</sub>");
	$output = Lang.Strings.Replace($output, "\r\n", "\n");
	$output = Lang.Strings.Replace($output, "\n", "<br/>\n");
	return $output;
}

String Ticket_GetUser(Integer $UID)
{
	if( $UID == 0 )
	{
		return "None";
	}
	return "UID #"+$UID;
}

# vim: ft=php
