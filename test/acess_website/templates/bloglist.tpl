{foreach $NewsItems as $Item}
<span class="blog_title"><a href="blog.php?post={$Item.pid}">{$Item.name}</a></a></span>
<span class="blog_byline">Posted {$Item.posted} by {$Item.username}</span><br/>
<p>
{$Item.formatted_text}
</p>
{foreachelse}
No posts
{endforeach}
