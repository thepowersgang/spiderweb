#!/usr/local/bin/sw
#<?php

include "database.inc.ss";

SpiderWeb.Template $tpl = new SpiderWeb.Template();

// Get the page to visit once login/logout is done
String $lLastPage = CGI.ReadGET("lastpage");
if( $lLastPage === null ) {
	$lLastPage = CGI.ReadPOST("lastpage");
}
if( $lLastPage === null ) {
	$lLastPage = "index.ss";
}
$tpl->Assign("lLastPage", $lLastPage);

// Check for logout
if( CGI.ReadGET("logout") !== null )
{
	// $gSession->Unset("uid");
	// $gSession->Unset("username");
	// CGI.ClearCookie("uid");
	// CGI.ClearCookie("auth");

	$tpl->Assign("PageTitle", "Logout");
	$tpl->Assign("PageID", "logout");
	$tpl->Display("templates/logout.tpl");
	return 0;
}

String $lUsername = "";
if( CGI.ReadPOST("username") !== null )
{
	$lUsername = CGI.ReadPOST("username");
	String $lPassword = CGI.ReadPOST("password");
	Boolean $lSetCookie = CGI.ReadPOST("use_cookie") !== null;
	
	/*
	// Hash password
	$lPassword = Hashes.MD5($lPassword);
	
	String $query = "SELECT `uid`,`magic` FROM `users`"
		+" WHERE `username`='"+$dbconn->Escape($lUsername)+"'"
		+" AND `password`=UNHEX('$lPassword')"
		+" LIMIT 1";
	*/
	String $query = "";
	SpiderWeb.MySQL.Result $r = $dbconn->Query($query);
	if( $r === null ) {
		IO.Print("MySQL Query failed, query was " + $query + "<br/>\n");
		IO.Print("Reason: " + $dbconn->LastErrorString() + "<br/>\n\n");
		return 0;
	}

	String $row[] = $r->GetNextRow();
	if( $row !== null )
	{
		// $gSession->Set("uid", $row[0]);
		// $gSession->Set("username", $lUsername);
		$tpl->Display("templates/login_success.tpl");
		return 0;
	}
}

$tpl->Assign("PageTitle", "Login");
$tpl->Assign("PageID", "login");
$tpl->Assign("Username", $lUsername);

$tpl->Display("templates/login.tpl");

# vim: ft=php
