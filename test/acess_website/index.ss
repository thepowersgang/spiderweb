#!/usr/local/bin/sw
#<?php

include "database.inc.ss";
include "blog.inc.ss";

SpiderWeb.Template $tpl = new SpiderWeb.Template();

$tpl->Assign("PageTitle", "Index");
$tpl->Assign("PageID", "index");

$tpl->Display("templates/header.tpl");

Lang.StringMap	$posts[];
$posts = Blog_GetLatest($dbconn, 0, 3);

/*
Lang.StringMap	$post = new Lang.StringMap();
$post->set("id", "1");
$post->set("name", "Testing Post");
$post->set("posted", "2012-06-23 00:01");
$post->set("username", "thePowersGang");
$post->set("formatted_text", "Test post content");
Lang.StringMap $posts[1];
$posts[0] = $post;
*/
$tpl->Assign("NewsItems", $posts);

$tpl->Display("templates/index.tpl");

#IO.Print("Page ID = " + CGI.ReadGET("page") + "</br>\n");
$tpl->Display("templates/footer.tpl");

# vim: ft=php
