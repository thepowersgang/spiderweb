#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");
$tpl->Assign("PageTitle", "Issue Summary");

String $query = "SELECT prj_sname,prj_name FROM ticket_projects LIMIT 10";
SpiderWeb.MySQL.Result $res = $dbconn->Query($query);
if( $res === null ) {
	$tpl->Assign("SQLQuery", $query);
	$tpl->Assign("SQLError", $dbconn->LastErrorString());
	$tpl->Display("templates/sqlerror.tpl");
	return 0;
}

Lang.StringMap[]	$laProjects($res->ResultCount()+1);
String[]	$row;
for( Integer $i = 0; ($row = $res->GetNextRow()); $i ++ )
{
	Lang.StringMap	$prj();

	$prj->set("ID", $row[0]);
	$prj->set("Name", $row[1]);

	// Count each bug type
	String	$q2 = "SELECT ticket_type,ticket_status,COUNT(*) FROM tickets WHERE ticket_project='"+$row[0]+"' GROUP BY ticket_type,ticket_status";
	SpiderWeb.MySQL.Result $r2 = $dbconn->Query($q2);
	if( $r2 === null ) {
		$tpl->Assign("SQLQuery", $q2);
		$tpl->Assign("SQLError", $dbconn->LastErrorString());
		$tpl->Display("templates/sqlerror.tpl");
		return 0;
	}

	// Get counts of each ticket type
	String[] $row2;
	while( $row2 = $r2->GetNextRow() )
	{
		$prj->set($row2[0]+"_"+$row2[1], $row2[2]);
	}
	
//	IO.Print("$i = "+$i);
	$laProjects[$i] = $prj;
}

$tpl->Assign("lBugProjects", $laProjects);
$tpl->Display("templates/bugs.tpl");

# vim: ft=php
