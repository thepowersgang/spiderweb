#!/usr/local/bin/sw
#<?php

include "database.inc.ss";
include "blog.inc.ss";

SpiderWeb.Template $tpl = new SpiderWeb.Template();

Lang.StringMap[]	$posts;
$posts = Blog_GetLatest($dbconn, 0, 10);

$tpl->Assign("LastPostTimestamp", $posts[sizeof($posts)-1]->get("posted"));

$tpl->Assign("NewsItems", $posts);
$tpl->Display("templates/blog.rss.tpl");

# vim: ft=php
