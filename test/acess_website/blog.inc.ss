#<?php

Lang.StringMap[] Blog_GetLatest(SpiderWeb.MySQL $dbconn, Integer $page, Integer $count)
{
	String $q = "SELECT pid,uid,posted,title,contents FROM blog"
		+" ORDER BY posted DESC"
		+" LIMIT " + ($page*$count) + "," + ($count+1);
	
	SpiderWeb.MySQL.Query $r = $dbconn->Query($q);
	
	return $r->GetAll();
}

# vim: ft=php
