{foreach NewsItems as Item}
<span class="blog_title"><a href="blog.php?post={$Item.id}">{$Item.name}</a></a></span><br/>
<span class="blog_byline">Posted {$Item.posted} by {$Item.username}</span><br/>
<p>
{$Item.formatted_text}
</p>
{/foreach}

<!-- vim: ft=html
-->
