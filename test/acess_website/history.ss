#!/usr/local/bin/sw
#<?php

SpiderWeb.Template $tpl = new SpiderWeb.Template();

$tpl->Assign("PageTitle", "History");
$tpl->Assign("PageID", "history");

$tpl->Display("templates/history.tpl");

# vim: ft=php
