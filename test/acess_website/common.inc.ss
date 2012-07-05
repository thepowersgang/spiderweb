#<?php

include "database.inc.ss";

CGI.Session $gSession("");
String $gUserName = $gSession->Get("username");
Integer $gUserID = (Integer)$gSession->Get("uid");

if( $gUserName === null )
	$gUserName = "";

SpiderWeb.Template $tpl();
$tpl->Assign("UserName", $gUserName);


# vim: ft=php
