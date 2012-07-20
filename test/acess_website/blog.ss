#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "blog");

Integer $lPostID = (Integer) CGI.ReadGET("post");

SpiderWeb.MySQL.Result $r;
if( $lPostID > 0 )
{
	String $q = "SELECT pid,blog.uid,username,posted,title,contents FROM blog,users"
		+" WHERE blog.uid=users.uid AND pid = " + $lPostID
		+" LIMIT 1";
	$r = $dbconn->Query($q);
}

if( $r !== null )
{
	String[] $row = $r->GetNextRow();
	
	Lang.StringMap	$post();
	$post->set("pid",      $row[0]);
	$post->set("uid",      $row[1]);
	$post->set("username", $row[2]);
	$post->set("posted",   $row[3]);
	$post->set("name",     $row[4]);
	$post->set("formatted_text", $row[5]);
	
	$tpl->Assign("lPostData", $post);
}

$tpl->Display("templates/blogpost.tpl");

# vim: ft=php
