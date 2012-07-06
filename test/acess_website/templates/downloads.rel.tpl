<a href="/Downloads/Acess2/Acess2_{$lRelease.ID}.tar.gz">Source Code</a> [tar.gz]<br />
Binaries
<ul>
 <li>
  <b>x86</b><br/>
  {if $lRelease.x86_bin_path}<a href="{$lRelease.x86_bin_path}">Prebuilt Tree</a>  [tar.gz]<br/>{endif}
  {if $lRelease.x86_img_path}<a href="{$lRelease.x86_img_path}">Floppy Image</a>  [img.gz]<br/>{endif}
 </li>
 <li>
  <b>x86-64</b><br/>
  {if $lRelease.x86_64_bin_path}<a href="{$lRelease.x86_64_bin_path}">Prebuilt Tree</a>  [tar.gz]<br/>{endif}
  {if $lRelease.x86_64_img_path}<a href="{$lRelease.x86_64_img_path}">Floppy Image</a>  [img.gz]<br/>{endif}
 </li>
</ul>
{if $lRelease.FullNotes != ""}
Release Notes:
<pre>
{$lRelease.FullNotes}
</pre>
{endif}

