{assign $lAltStylesheet="style.tickets.css"}
{include "header.tpl"}

      <a href="bugs.tpl">Tickets</a>
-&gt; <a href="ticket_list.ss?prj={$lTicket.project}">{$lTicket.project}</a>
-&gt; <a href="ticket_list.ss?prj={$lTicket.project}&amp;type={$lTicket.type}">{$lTicket.type}</a>
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
  <td>{$lTicket.opened} by {$lTicket.filer|tickets_userlink}</td>
 </tr>
 <tr>
  <th>Owner</th>
  <td>{$lTicket.owner|tickets_userlink}</td>
 </tr>
 <tr>
  <td colspan="2">
{$lTicket.desc}
  </td>
 </tr>
</table>

<div class="ticket_comments">
{foreach $lComments as $comment}
<div class="ticket_comment">
<span class="time">{$comment.time}</span>
<span class="user">{$comment.user|tickets_userlink}</span>
<span class="number">{$comment.num}</span>
<div class="text">
{$comment.text}
</div>
</div>
{foreachelse}
<b>No comments yet</b>
{endforeach}
</div>

{if $UserName != ""}
<form method="POST" action="ticket_edit.ss?a=update">
 <div class="ticket_postcomment">
  Post comment: <br />
  <textarea name="comment_text" rows="5" cols="30"></textarea>
 </div>
 <div class="ticket_edit">
  <input type="radio" name="action" value="none" checked="1"/>
  No Change
  <br />
{if $lTicket.status == "closed"}
  <input type="radio" name="action" value="reopen" />
  Re-open
  <br />
{elseif $lTicket.status == "reviewing"}
  <input type="radio" name="action" value="close" />
  Close
  <br />
  <input type="radio" name="action" value="needswork" />
  Needs work
  <br />
{else}
  <input type="radio" name="action" value="fixed" />
  Fixed
  <br />
  <input type="radio" name="action" value="invalid" />
  Invalid
  <br />
{endif}
 </div>
</form>
{endif}

{include "footer.tpl"}
