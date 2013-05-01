#!/usr/local/bin/sw
#<?php

include "common.inc.ss";
$tpl->Assign("PageID", "downloads");

void _set_file_key(Lang.StringMap $map, String $key, String $file)
{
	String $lBasePath = "/Downloads/Acess2";
	String $lWebRootBase = "/home/http/www";
	
	String $path = $lBasePath + "/" + $file;
	IO.File $fp($lWebRootBase + $path, "r");
	if( $fp !== null ) {
		$map->set($key, $path);
		delete $fp;
	}
}

Lang.StringMap _loadRelease(String $ID)
{
	// Fill in implicit fields	
	Lang.StringMap	$ret();
	$ret->set("ID", $ID);
	String $name = Lang.Strings.SubStr($ID, 0, sizeof($ID)-2)+"."+Lang.Strings.SubStr($ID, sizeof($ID)-2, 2);
	$ret->set("Name", $name);

	// Load	release notes
	IO.File	$fp("releases/"+$ID+".txt", "r");
	if( $fp === null ) {
		return null;
	}
	String $notes = $fp->Read(2000);
	$ret->set("Desc", Lang.Strings.SubStr($notes, 0, Lang.Strings.Search($notes, "\n")));
	$ret->set("FullNotes", $notes);
	delete $fp;
	
	// Get avaliable builds
	_set_file_key($ret, "x86_bin_path", "Acess2_"+$ID+"_x86_bin.tar.gz");	// - x86 Bin
	_set_file_key($ret, "x86_img_path", "Acess2_"+$ID+"_x86.img.gz");	// - x86 Floppy
	_set_file_key($ret, "x86_64_bin_path", "Acess2_"+$ID+"_x86-64_bin.tar.gz");	// - x86-64 Bin
	_set_file_key($ret, "x86_64_img_path", "Acess2_"+$ID+"_x86-64.img.gz");	// - x86-64 Floppy

	// Return data
	return $ret;
}

IO.File $fp("releases/LATEST", "r");
String $lLatestID = $fp->Read(100);
delete $fp;
// TODO Fix this to handle releases > 1.00
$lLatestID = "0"+(Integer)$lLatestID;

String $lRequestedRelease = CGI.ReadGET("id");
if( $lRequestedRelease === null )
{
	$tpl->Assign("PageTitle", "Downloads");

	// Load the description of the latest release
	Lang.StringMap	$latest = _loadRelease($lLatestID);
	if( $latest ) {
		$latest->set("FullNotes", "");	// Clear the release notes
		$tpl->Assign("lRelease", $latest);
	}

	// Get all the previous releases in the release directory
	Integer $nReleases = 0;
	String[] $releases( (Integer)$lLatestID );
	
	IO.Directory $dp("releases");
	if( $dp === null )
	{
		IO.Print("Unable to open releases directory");
	}
	else
	{
		String $file;
		while( ($file = $dp->Read()) !== null )
		{
			if( sizeof($file) != 7 )
				continue ;
			if( Lang.Strings.SubStr($file, 3, 4) != ".txt" )
				continue ;
			$releases[$nReleases] = Lang.Strings.SubStr($file, 0, 3);
			$nReleases ++;
		}
	}
	delete $dp;

	// Sort releases
	$releases = Lang.SortStrings($releases);

	// Load the description for each previous release
	Lang.StringMap[] $lPreviousReleases ($nReleases);
	for( Integer $i = 0; $i < $nReleases; $i ++ )
	{
		$lPreviousReleases[$i] = _loadRelease( $releases[$i] );
	}
	$tpl->Assign("lPreviousReleases", $lPreviousReleases);
	
	$tpl->Display("templates/downloads.tpl");
}
else
{
	Lang.StringMap	$rel;
	// Make sure that the release ID is valid
	if( 0 < (Integer)$lRequestedRelease && (Integer)$lRequestedRelease <= (Integer)$lLatestID ) {
		$rel = _loadRelease( $lRequestedRelease );
	}

	// If the release ID was invalid, then we need to set a different title
	if( $rel !== null ) {
		$tpl->Assign("PageTitle", "Downloads - Release "+$rel->get("Name"));
	}
	else {
		$tpl->Assign("PageTitle", "Downloads - Bad Release ID");
	}

	$tpl->Assign("lRelease", $rel);
	$tpl->Display("templates/header.tpl");
	$tpl->Display("templates/downloads.rel.tpl");
	$tpl->Display("templates/footer.tpl");
}

# vim: ft=php
