#<?php

// TODO: Direct execution guard

global SpiderWeb.MySQL $dbconn = new SpiderWeb.MySQL("localhost", "acess2sw", "acess2sw");
$dbconn->SetDatabase("acess2website");

SpiderWeb.MySQL.Result DB_Query(String $query)
{
	global SpiderWeb.Template $tpl;
	global SpiderWeb.MySQL $dbconn;
	SpiderWeb.MySQL.Result $res = $dbconn->Query($query);
	if( $res === null ) {
		$tpl->Assign("SQLQuery", $query);
		$tpl->Assign("SQLError", $dbconn->LastErrorString());
		$tpl->Display("templates/sqlerror.tpl");
		exit();
	}
	return $res;
}

String[] DB_GetSingleRow(String $query)
{
	SpiderWeb.MySQL.Result $res = DB_Query($query);
	return $res->GetNextRow();
}

# vim: ft=php
