#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "bugs");
$tpl->Assign("PageTitle", "Issue Summary");

String $query = "SELECT id,sname,name FROM bug_cats";
SpiderWeb.MySQL.Result $res = $dbconn->Query($query);
if( $res === null ) {
	$tpl->Assign("SQLQuery", $query);
	$tpl->Assign("SQLError", $dbconn->LastErrorString());
	$tpl->Display("templates/sqlerror.tpl");
	return 0;
}

Lang.StringMap[]	$laProjects($res->ResultCount());
String[]	$row;
for( Integer $i = 0; ($row = $res->GetNextRow()); $i ++ )
{
	Lang.StringMap	$prj();

	$prj->set("ID", $row[1]);	
	$prj->set("Name", $row[2]);

	// Count each bug type
	String	$q2 = "SELECT type,status,COUNT(*) FROM bug_reports WHERE cat="+$row[0]+" GROUP BY type,status";
//	String	$q2 = "SELECT type,status,COUNT(*) FROM tickets WHERE cat="+$row[0]+" GROUP BY type,status";
	SpiderWeb.MySQL.Result $r2 = $dbconn->Query($q2);
	if( $r2 === null ) {
		$tpl->Assign("SQLQuery", $q2);
		$tpl->Assign("SQLError", $dbconn->LastErrorString());
		$tpl->Display("templates/sqlerror.tpl");
		return 0;
	}

	Lang.StringMap	$bugCounts();
	Lang.StringMap	$featCounts();

	// TODO Get counts of open tickets
	String[] $row2;
	while( $row2 = $r2->GetNextRow() )
	{
		$prj->set($row2[0]+"_"+$row2[1], $row2[2]);
	}
	
	$laProjects[$i] = $prj;
}


$tpl->Assign("lBugProjects", $laProjects);
$tpl->Display("templates/bugs.tpl");

# vim: ft=php
