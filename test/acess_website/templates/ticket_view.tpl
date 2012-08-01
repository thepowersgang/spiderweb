{include "header.tpl"}

      <a href="bugs.tpl">Tickets</a>
-&gt; <a href="ticket_list?prj={$lTicket.project}">{$lTicket.project}</a>
-&gt; <a href="ticket_list?prj={$lTicket.project}&amp;type={$lTicket.type}">{$lTicket.type}</a>
-&gt; <b>{$lTicket.title}</b>

<table>
 <tr>
  <th>Title</th>
  <td>{$lTicket.title}</td>
 </tr>
 <tr>
  <th>Status</th>
  <td>{$lTicket.status}</td>
 </tr>
 <tr>
  <th>Filed</th>
  <td>{$lTicket.opened} by {$lTicket.filer}</td>
 </tr>
 <tr>
  <th>Owner</th>
  <td>{$lTicket.owner}</td>
 </tr>
 <tr>
  <td colspan="2">
{$lTicket.desc}
  </td>
 </tr>
</table>

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
