{include "header.tpl"}

{if $lPostData}
<h2>{$lPostData.name}</h2>
<span class="blog_byline">Posted {$lPostData.posted} by {$lPostData.username}</span><br/>
<p>
{$lPostData.text|formatText}
</p>
{else}
Invalid post ID
{endif}

{include "footer.tpl"}
