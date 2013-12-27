{assign $lAltStylesheet="style.tickets.css"}
{include "header.tpl"}

      <a href="{$SiteRoot}bugs.ss">Tickets</a>
-&gt; <a href="{$SiteRoot}ticket_list.ss/{$lTicket.project}">{$lTicket.project}</a>
-&gt; <a href="{$SiteRoot}ticket_list.ss/{$lTicket.project}/{$lTicket.type}">{$lTicket.type}</a>
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
{$lTicket.desc|tickets_formattext}
  </td>
 </tr>
</table>

<div class="ticket_comments">
{foreach $lComments as $comment}
<div class="ticket_comment">
<span class="time">{$comment.time}</span>
<span class="user">Posted by {$comment.user|tickets_userlink}</span>
<span class="number">Comment #{$comment.num}</span>
<div class="text">
{$comment.text|tickets_formattext}
</div>
</div>
{foreachelse}
<b>No comments yet</b>
{endforeach}
</div>

{if $UserName != ""}
<form method="POST" action="{$SiteRoot}ticket_edit.ss?id={$lTicketID}&amp;a=update">
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
{else} {* new/accepted/needswork *}
  <input type="radio" name="action" value="resolve" />
  Resolve as
  <select name="resolution">
   <option value="fixed">fixed</option>
   <option value="wontfix">wontfix</option>
   <option value="cnr">CNR</option>
   <option value="invalid">not a bug</option>
   <option value="duplicate">duplicate</option>
  </select>
  (move to reviewing)
  <br />
 {if $lTicket.status == "new"}
  <input type="radio" name="action" value="accept" />
  Accept ticket
  <br />
 {endif}
{endif}
 </div>
 <input type="submit" name="subtype" value="Submit changes" />
</form>
{endif}

{include "footer.tpl"}
