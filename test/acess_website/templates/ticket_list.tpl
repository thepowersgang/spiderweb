{include "header.tpl"}

      <a href="bugs.ss">Tickets</a>
{if $lType!=""}
-&gt; <a href="ticket_list.ss?prj={$lProject}">{$lProject}</a>
-&gt; <b>{$lType}</b>
{else}
-&gt; <b>{$lProject}</b>
{endif}
-&gt; <a href="ticket_file.ss?{$lTypeCodes}">New</a><br/>

<table width="100%">
<tr>
 <th width="30"><a href="?{$lTypeCodes}sort=id" class="{?$lSort=="id"?"selected":""}">ID</a></th>
 <th width="60">Type</th>
 <th width="60">Status</th>
 <th><a href="?{$lTypeCodes}sort=title" class="{?$lSort=="title"?"selected":""}">Short Description</a></th>
 <th width="150"><a href="?{$lTypeCodes}sort=filed" class="{?$lSort=="filed"?"selected":""}">Filed</a></th>
 <th width="60">Owner</th>
</tr>
{foreach $lTicketList as $ticket}
<tr class="ticket_{$ticket.status}">
 <td>{$ticket.ID}</td>
 <td>{$ticket.type}</td>
 <td>{$ticket.status}</td>
 <td><a href="ticket_view.ss?id={$ticket.ID}">{$ticket.title}</a></td>
 <td>{$ticket.opened}<br/> {$ticket.filer|tickets_userlink}</td>
 <td><a href="users.ss?id={$ticket.owner_id}">{$ticket.owner|tickets_userlink}</a></td>
</tr>
{foreachelse}
<tr>
 <td colspan="99">No tickets</td>
</tr>
{endforeach}
</table>

{include "footer.tpl"}
