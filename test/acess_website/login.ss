#!/usr/local/bin/sw
#<?php

include "common.inc.ss";

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
	$gSession->Unset("uid");
	$gSession->Unset("username");
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
	
	// Hash password
	// $lPassword = Crypto.MD5($lPassword);
	
	String $query = "SELECT `uid`,`magic` FROM `users`"
		+" WHERE `username`='"+$dbconn->Escape($lUsername)+"'"
		+" AND `password`=UNHEX(MD5('"+$dbconn->Escape($lPassword)+"'))"
		+" LIMIT 1";
	SpiderWeb.MySQL.Result $r = $dbconn->Query($query);
	if( $r === null ) {
		$tpl->Assign("SQLQuery", $query);
		$tpl->Assign("SQLError", $dbconn->LastErrorString());
		$tpl->Display("templates/sqlerror.tpl");
		return 0;
	}

	String[] $row = $r->GetNextRow();
	if( $row !== null )
	{
		$gUserID = (Integer)$row[0];
		$gUserName = $lUsername;
		String $token = RandString(12);
		String $q = "UPDATE users SET `token`='"+$dbconn->Escape($token)+"',`token_age`=NOW() WHERE uid="+$gUserID+" LIMIT 1";
		if( $dbconn->Query($q) === null ) {
			$tpl->Assign("SQLQuery", $q);
			$tpl->Assign("SQLError", $dbconn->LastErrorString());
			$tpl->Display("templates/sqlerror.tpl");
			return 0;
		}

		$gSession->Set("uid", $row[0]);
		$gSession->Set("username", $lUsername);
		CGI.SetCookie("A2SW_USERNAME", $lUsername, 3600);
		CGI.SetCookie("A2SW_AUTHTOK", $token, 3600);
		$tpl->Display("templates/login_success.tpl");
		return 0;
	}
}

$tpl->Assign("PageTitle", "Login");
$tpl->Assign("PageID", "login");
$tpl->Assign("lUsername", $lUsername);

$tpl->Display("templates/login.tpl");

# vim: ft=php
