#<?php

// TODO: Direct execution guard

SpiderWeb.MySQL $dbconn = new SpiderWeb.MySQL("localhost", "acess2sw", "acess2sw");
$dbconn->SetDatabase("acess2website");

SpiderWeb.MySQL.Result DB_Query(SpiderWeb.Template $tpl, SpiderWeb.MySQL $dbconn, String $query)
{
	SpiderWeb.MySQL.Result $res = $dbconn->Query($query);
	if( $res === null ) {
		$tpl->Assign("SQLQuery", $query);
		$tpl->Assign("SQLError", $dbconn->LastErrorString());
		$tpl->Display("templates/sqlerror.tpl");
		exit();
	}
	return $res;
}

String[] DB_GetSingleRow(SpiderWeb.Template $tpl, SpiderWeb.MySQL $dbconn, String $query)
{
	SpiderWeb.MySQL.Result $res = DB_Query($tpl, $dbconn, $query);
	return $res->GetNextRow();
}

# vim: ft=php
