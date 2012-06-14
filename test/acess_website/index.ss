#!/usr/bin/sw
#<?php

#include "database.inc.ss"

SpiderWeb.Template $tpl = new SpiderWeb.Template();

$tpl->Assign("PageTitle", "Index");
$tpl->Assign("PageID", "index");

$tpl->Display("templates/header.tpl");
$tpl->Display("templates/index.tpl");
$tpl->Display("templates/footer.tpl");

# vim: ft=php
