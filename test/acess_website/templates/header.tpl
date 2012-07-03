<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<!DOCTYPE html
	PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
 <title>AcessOS - {$PageTitle}</title>
 <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
 <meta name="Generator" content="vim, SpiderScript, and Elbow Grease" />
 <meta name="Author" content="John Hodge (thePowersGang) - 20518201" />
 <meta name="Keywords" content="acess acess2 acessos thepowersgang operating-system" />
 <link rel="stylesheet" type="text/css" href="style.css" />
 <script type="text/javascript" src="script.js"></script>
 <link rel="alternate" title="Acess2 News RSS" href="blog.rss.sw" type="application/rss+xml" />
</head>
<body>
 <div id="header">
  <h1>Acess OS Project</h1>
  <center>Lightweight and extensible Operating System</center>
 </div>
 <div id="menubar"><div>
  <a href="index.ss" class="{?$PageID=="index"?"selected":""}">Home</a>
  <a href="history.ss" class="{?$PageID=="history"?"selected":""}">History</a>
  <a href="downloads.ss" class="{?$PageID=="downloads"?"selected":""}">Downloads</a>
  <a href="docs.ss" class="{?$PageID=="docs"?"selected":""}">Documentation</a>
  <a href="screens.ss" class="{?$PageID=="screens"?"selected":""}">Pictures</a>
  <a href="bugs.ss">Bug Reports</a>
{* <a href="forums.php">Forums</a> *}
  <span class="right">
{if $UserName!=""}
   Welcome <i>{$UserName}</i>
   <a href="usercp.ss">User CP</a>
   <a href="login.ss?logout&amp;lastpage={$PagePathEnc}">Logout</a>
{else}
   <a href="login.ss?lastpage={$PagePathEnc}">Login</a>
   <a href="register.ss?lastpage={$PagePathEnc}">Register</a>
{endif}
  </span>
 </div></div>
 <div id="body">
{if $PageID != "index"}
<h2>{$PageTitle}</h2>
{endif}

{* vim: ft=html
*}
