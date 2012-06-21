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
$tpl->Assign("NewsItems", $posts);

$tpl->Display("templates/index.tpl");

#IO.Print("Page ID = " + CGI.ReadGET("page") + "</br>\n");
$tpl->Display("templates/footer.tpl");

# vim: ft=php
