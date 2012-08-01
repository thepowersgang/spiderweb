{include "header.tpl"}

<table>
<tr>
 <th><a href="{$lSortBase}id" class="{?$lSort=="id"?"selected":""}">ID</a></th>
 <th>Type</th>
 <th>Status</th>
 <th><a href="{$lSortBase}title" class="{?$lSort=="title"?"selected":""}">Short Description</a></th>
 <th><a href="{$lSortBase}filed" class="{?$lSort=="filed"?"selected":""}">Filed</a></th>
 <th>Owner</th>
</tr>
{foreach $lTicketList as $ticket}
<tr class="ticket_{$ticket.status}">
 <td><a href="ticket_view.ss?id={$ticket.ID}">{$ticket.ID}</a></td>
 <td>{$ticket.type}</td>
 <td>{$ticket.status}</td>
 <td>{$ticket.title}</td>
 <td>{$ticket.opened} by {$ticket.filer_name}</td>
 <td><a href="users.ss?id={$ticket.owner_id}">{$ticket.owner_name}</a></td>
</tr>
{foreachelse}
<tr>
 <td colspan="99">No tickets</td>
</tr>
{endforeach}
</table>

{include "footer.tpl"}
