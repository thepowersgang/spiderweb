{include "header.tpl"}

{foreach $lBugProjects as $prj}
<h2>{$prj.Name}</h2>
<table>
<tr><th /><th>Open</th><th>Fixed</th><th>Won't Fix</th></tr>
<tr>
 <th><a href="ticket_list.ss?prj={$prj.ID}&amp;type=bug">Bugs</a></th>
 <td>{$prj.bug_open    ? $prj.bug_open   :"0"}</td>
 <td>{$prj.bug_fixed   ? $prj.bug_fixed  :"0"}</td>
 <td>{$prj.bug_wontfix ? $prj.bug_wontfix:"0"}</td>
</tr>
<tr>
 <th><a href="ticket_list.ss?prj={$prj.ID}&amp;type=bug">Features</a></th>
 <td>{$prj.feat_open   ?$prj.feat_open   :"0"}</td>
 <td>{$prj.feat_fixed  ?$prj.feat_fixed  :"0"}</td>
 <td>{$prj.feat_wontfix?$prj.feat_wontfix:"0"}</td>
</tr>
</table>
{endforeach}

{include "footer.tpl"}
