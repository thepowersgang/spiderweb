#!/usr/local/bin/sw
#<?php

SpiderWeb.Template $tpl = new SpiderWeb.Template();

$tpl->Assign("PageTitle", "History");
$tpl->Assign("PageID", "history");

$tpl->Display("templates/header.tpl");
$tpl->Display("templates/history.tpl");
$tpl->Display("templates/footer.tpl");

# vim: ft=php
