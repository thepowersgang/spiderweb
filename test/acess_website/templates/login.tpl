{include "header.tpl"}
{if $lUsername != ""}
<span class="error">Invalid username or password</span>
{endif}
<form action="?" method="post">
 <input type="hidden" name="lastpage" value="{$lLastPage}" />
 <table>
  <tr>
   <th>Username</th>
   <td><input name="username" value="{$lUsername}" /></td>
  </tr>
  <tr>
   <th>Password</th>
   <td><input type="password" name="password" /></td>
  </tr>
 </table>
 Remember Me? <input type="checkbox" name="cookie" value="1" />
 (Requires cookies enabled)<br/>
 <input type="submit" value="Login" />
</form>
{include "footer.tpl"}
