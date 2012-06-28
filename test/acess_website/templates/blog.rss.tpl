<?xml version="1.0"?>
<rss version="2.0" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:atom="http://www.w3.org/2005/Atom">
 <channel>
  <title>Acess OS Project - Blog</title>
  <link>http://www.mutabah.net/</link>
  <atom:link rel="self" href="http://www.mutabah.net/acess2/blog.rss.php" type="application/rss+xml"/>
  <description>Updates on the progress of the Acess Hobby OS project</description>
  <language>en-au</language>
  <pubDate>{$LastPostTimestamp}</pubDate>

  <lastBuildDate>{$LastPostTimestamp}</lastBuildDate>
  <docs>http://cyber.law.harvard.edu/rss/rss.html</docs>
  <generator>Acess Project CMS (SpiderScript)</generator>
  <webMaster>webmaster@mutabah.net (thePowersGang)</webMaster>
{foreach $NewsItems as $Item}
  <item>
   <title>{$Item.name}</title>
   <link>http://www.mutabah.net/acess2/blog.php?post={$Item.pid}</link>
   <pubDate>{$Item.posted}</pubDate>
   <guid>http://www.mutabah.net/acess2/blog.php/{$Item.posted}/{$Item.pid}</guid>
   <dc:creator>{$Item.username}</dc:creator>
   <description>{$Item.formatted_text}</description>
  </item>
{endforeach}
 </channel>
</rss>

