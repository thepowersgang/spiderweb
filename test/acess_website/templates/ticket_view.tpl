{include "header.tpl"}

<div class="ticket_info">
{* <span class="ticket_title">{$lTicket.title}</span> *}
<span class="ticket_opened">{$lTicket.opened}</span>
<span class="ticket_filer">Filed by {$lTicket.filer}</span>
<span class="ticket_owner">Owned by {$lTicket.owner}</span>
<div class="ticket_desc">
{$lTicket.desc}
</div>
</div>

{foreach $lComments as $comment}
<div class="ticket_comment">
<span class="time">{$comment.time}</span>
<span class="user">{$comment.user}</span>
<span class="number">{$comment.num}</span>
<div class="text">
{$comment.text}
</div>
</div>
{endforeach}

{include "footer.tpl"}
