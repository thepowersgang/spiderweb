#<?php

include "database.inc.ss";
global SpiderWeb.Template $tpl();

String $gSessionHash = CGI.GetCookie("SWSESSIONID");
CGI.Session $gSession($gSessionHash, "acess2sw");
//if($gSessionHash != $gSession->Hash) {
//	$gSessionHash = $gSession->Hash;
//	CGI.SetCookie("SWSESSIONID", $gSessionHash, 3600);
//}
String $gUserName = $gSession->Get("username");
Integer $gUserID = (Integer)$gSession->Get("uid");

if( $gUserName === null )
{
	$gUserName = CGI.GetCookie("A2SW_USERNAME");
	String $authtoken = CGI.GetCookie("A2SW_AUTHTOK");
	if( $gUserName !== null && $authtoken !== null )
	{
		String $q = "SELECT uid FROM users WHERE"
			+" `username`='"+$dbconn->Escape($gUserName)+"'"
			+" AND `token`='"+$dbconn->Escape($authtoken)+"'"
			+" AND `token_age` > NOW() - INTERVAL 1 DAY";
		String[] $row = DB_GetSingleRow($q);
		if($row !== null)
		{
			$gUserID = (Integer)$row[0];
		}
		else
		{
		//	IO.Print("Bad cookie username/token "+$gUserName+"/"+$authtoken);
		}
	}
}

if( $gUserID == 0 || $gUserName === null )
	$gUserName = "";

$tpl->Assign("UserName", $gUserName);

$tpl->BindFilter("formatText", "formatText");

String formatText(String $Input)
{
	String $output = $Input;
	
#	$output = Lang.Strings.RegexReplace($output, "^.*^", "<sup>$1</sup>");
#	$output = Lang.Strings.RegexReplace($output, "_.*_", "<sub>$1</sub>");
	$output = Lang.Strings.Replace($output, "\r\n", "\n");
	$output = Lang.Strings.Replace($output, "&", "&amp;");
	$output = Lang.Strings.Replace($output, "<", "&lt;");
	$output = Lang.Strings.Replace($output, ">", "&gt;");
	$output = Lang.Strings.Replace($output, "{{{", "<span style='font-size:small;font-family:mono'>");
	$output = Lang.Strings.Replace($output, "}}}", "</span>");
	$output = Lang.Strings.Replace($output, "\n", "<br/>\n");
	
	return $output;
}

String Ticket_GetUser(SpiderWeb.MySQL $dbconn, Integer $UID)
{
	String	$name;
	if( $UID == 0 )
	{
		$name = "None";
	}
	else
	{
		String $q = "SELECT username FROM users WHERE uid="+$UID;
		SpiderWeb.MySQL.Result $res = $dbconn->Query($q);
		String[] $row = $res->GetNextRow();
		if( $row ) 
			$name = $row[0];
		else {
			$name = "Unknown";
			$UID = -1;
		}
	}
	
	return "<a href=\"users.ss?id="+$UID+"\">"+$name+"</a>";
}

# vim: ft=php
