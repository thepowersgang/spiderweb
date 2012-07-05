#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
include "blog.inc.ss";

$tpl->Assign("PageTitle", "Index");
$tpl->Assign("PageID", "index");

$tpl->Display("templates/header.tpl");

Lang.StringMap	$posts[];
$posts = Blog_GetLatest($dbconn, 0, 3);
$tpl->Assign("NewsItems", $posts);

$tpl->Display("templates/index.tpl");

$tpl->Display("templates/footer.tpl");

# vim: ft=php
