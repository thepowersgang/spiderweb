{include "header.tpl"}

<p>
SQL Error<br/>
<tt>{$SQLError}</tt>
</p>
{if $SQLQuery}
Query was:<br/>
<tt>{$SQLQuery}</tt>
{endif}

{include "footer.tpl"}
