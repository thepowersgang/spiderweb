{include "header.tpl"}

      <a href="bugs.ss">Tickets</a>
-&gt; <a href="ticket_list.ss?prj={$lProject}">{$lProject}</a>
{if $lType!=""}
-&gt; <a href="ticket_list.ss?prj={$lProject}&amp;type={$lType}">{$lType}</a>
{endif}

<form action="?a=post" method="POST">
 <b>Title</b> <br />
 <input name="title" size="60" /> <br/>
{if $lType==""}
 <b>Type</b> <br />
 <select name="type">
  <option value="bug">Bug</option>
{*  <option value="defect">Defect</option> *}
  <option value="feature">Feature Request</option>
  <option value="discussion">Discussion</option>
 </select>
 <br />
{else}
 <input type="hidden" name="type" value="{$lType}" />
{endif}
 <input type="hidden" name="prj" value="{$lProject}" />
 <b>Description</b> <br />
 <textarea rows="10" cols="80" name="desc"></textarea>
 <br/>
 <input type="submit" value="Submit" />
</form>

{include "footer.tpl"}
