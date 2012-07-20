{include "header.tpl"}

<table>
<tr>
 <th>ID</th>
 <th>Title</th>
 <th>Type</th>
 <th>Status</th>
</tr>
{foreach $lTicketList as $ticket}
<tr>
 <td><a href="ticket_view.ss?id={$ticket.ID}">{$ticket.ID}</a></td>
</tr>
{foreachelse}
<tr>
 <td colspan="99">No tickets</td>
</tr>
{endforeach}
</table>

{include "footer.tpl"}
