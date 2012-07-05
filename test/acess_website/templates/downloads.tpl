{include "header.tpl"}
<h3>git Snapshot ({$lLatestGit})</h3>
<a href="{$gRelDir}/Acess2_git.tar.gz">git tree</a> [tar.gz]<br/>

<h3>Lastest Release ({$lRelease.Name})</h3>
{$lRelease.Desc}<br/>
<a href="downloads.ss?id={$lRelease.ID}">Release Notes</a><br/>
<br/>

{include "downloads.rel.tpl"}

<h3>Other Releases</h3>
<ul>
{foreach $lPreviousReleases as $rel}
<li><a href="downloads.ss?id={$rel.ID}">{$rel.Name}</a> - {$rel.Desc}</li>
{endforeach}
</ul>

{include "footer.tpl"}

