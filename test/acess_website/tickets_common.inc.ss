#!/usr/local/bin/sw
#<?php

String Ticket_GetUser(SpiderWeb.MySQL $dbconn, Integer $UID)
{
	global String $SiteRoot;
	
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
	
	return "<a href=\""+$SiteRoot+"users.ss?id="+$UID+"\">"+$name+"</a>";
}
String makeuserlink(String $uid)
{
	global SpiderWeb.MySQL $dbconn;
	return Ticket_GetUser($dbconn, (Integer)$uid);
}
$tpl->BindFilter("tickets_userlink", "makeuserlink");
$tpl->BindFilter("tickets_formattext", "formatText");

# vim: ft=php

