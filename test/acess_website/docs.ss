#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "docs");
$tpl->Assign("PageTitle", "Documentation");


$tpl->Display("templates/docs.tpl");

// vim: ft=php

