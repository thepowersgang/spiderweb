{include "header.tpl"}

{foreach $lBugProjects as $prj}
<h2>{$prj.Name}</h2>
<table>
<tr><th /><th>New</th><th>Accepted</th><th>Reviewing</th><th>Needs Work</th><th>Closed</th></tr>
<tr>
 <th><a href="ticket_list.ss?prj={$prj.ID}&amp;type=bug">Bugs</a></th>
 <td>{?$prj.bug_new      ? $prj.bug_new      :"0"}</td>
 <td>{?$prj.bug_accepted ? $prj.bug_accepted :"0"}</td>
 <td>{?$prj.bug_reviewing? $prj.bug_reviewing:"0"}</td>
 <td>{?$prj.bug_needswork? $prj.bug_needswork:"0"}</td>
 <td>{?$prj.bug_closed   ? $prj.bug_closed   :"0"}</td>
</tr>
<tr>
 <th><a href="ticket_list.ss?prj={$prj.ID}&amp;type=feat">Features</a></th>
 <td>{?$prj.feat_new      ? $prj.feat_new      :"0"}</td>
 <td>{?$prj.feat_accepted ? $prj.feat_accepted :"0"}</td>
 <td>{?$prj.feat_reviewing? $prj.feat_reviewing:"0"}</td>
 <td>{?$prj.feat_needswork? $prj.feat_needswork:"0"}</td>
 <td>{?$prj.feat_closed   ? $prj.feat_closed   :"0"}</td>
</tr>
<tr>
 <th><a href="ticket_list.ss?prj={$prj.ID}&amp;type=discussion">Discussions</a></th>
 <td>{?$prj.discussion_new      ? $prj.discussion_new      :"0"}</td>
 <td>{?$prj.discussion_accepted ? $prj.discussion_accepted :"0"}</td>
 <td>{?$prj.discussion_reviewing? $prj.discussion_reviewing:"0"}</td>
 <td>{?$prj.discussion_needswork? $prj.discussion_needswork:"0"}</td>
 <td>{?$prj.discussion_closed   ? $prj.discussion_closed   :"0"}</td>
</tr>
</table>
{endforeach}

{include "footer.tpl"}
