#<?php

Lang.StringMap[] Blog_GetLatest(SpiderWeb.MySQL $dbconn, Integer $page, Integer $count)
{
	String $q = "SELECT pid,blog.uid,username,posted,title,contents FROM blog,users"
		+" WHERE blog.uid=users.uid"
		+" ORDER BY posted DESC"
		+" LIMIT " + ($page*$count) + "," + ($count+1);
	
	SpiderWeb.MySQL.Result $r = $dbconn->Query($q);
	if( $r === null )
	{
		IO.Print("MySQL Query failed, query was " + $q + "<br/>\n");
		IO.Print("Reason: " + $dbconn->LastErrorString() + "<br/>\n\n");
		return null;
	}

	Integer	$post_count = $r->ResultCount();
	Lang.StringMap[] $posts($post_count);
	Integer $i;
	
	for( $i = 0; $i < $post_count; $i ++ )
	{
		String[] $row = $r->GetNextRow();
		if( $row === null )	continue;
			
		$posts[$i] = new Lang.StringMap();
		$posts[$i]->set("pid",      $row[0]);
		$posts[$i]->set("uid",      $row[1]);
		$posts[$i]->set("username", $row[2]);
		$posts[$i]->set("posted",   $row[3]);
		$posts[$i]->set("name",     $row[4]);
		$posts[$i]->set("formatted_text", $row[5]);
	}
	return $posts;
}

# vim: ft=php
