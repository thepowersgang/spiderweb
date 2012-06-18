#!/usr/local/bin/sw
#<?php

SpiderWeb.Template $tpl = new SpiderWeb.Template();

$tpl->Assign("PageTitle", "Index");
$tpl->Assign("PageID", "index");

$tpl->Display("templates/header.tpl");
$tpl->Display("templates/index.tpl");
#IO.Print("Page ID = " + CGI.ReadGET("page") + "</br>\n");
$tpl->Display("templates/footer.tpl");

# vim: ft=php
