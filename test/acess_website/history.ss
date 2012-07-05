#!/usr/local/bin/sw
#<?php

include "common.inc.ss";

$tpl->Assign("PageTitle", "History");
$tpl->Assign("PageID", "history");

$tpl->Display("templates/history.tpl");

# vim: ft=php
