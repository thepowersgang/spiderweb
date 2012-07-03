{include "header.tpl"}
Acess has an interesting history. It started out as a near fantasy idea
for a replacement for windows, written on a MacPlus, when I was about
eleven. In my designings, I made a basic programming language (very high
level) and designed the basic layout of the filesystem (using ; as a
path separator). This original version was going to be called <em>Access
OS</em>, but I was not very good at spelling. By the time I realised my
mistake, the name of Acess had stuck.<br />
This turned out to be a good thing, Acess is distinctive, and avoids any
risk of trademark collisions with a certain database application.<br />
<br />
These childish plans were all thrown out in early high school (2005) when
I discovered osdever.net and I began to actually write code for Acess.<br />
The original version, AcessOS, was heavily based on Bran's Kernel
Development tutorial. This version forked into two versions when I started
to work on multitasking. Since I wanted to be able to work on my already
near stable VFS code while still debugging the virtual memory and process
management, I kept a version that was single tasking, and ran completely
in physical memory. These two versions both shared the same common code
(namely, the driver and filesystem trees), leading to some very interesting
build scripts to allow these two to share code.<br />
<br />
After hacking at this for all of high school, with multiple breaks in
work due to large blocker bugs, I gave up. In early 2009 I began to work
on a microkernel that shared the Acess name. This didn't go very far (I
tend to like monolithic kernels, and find the idea of a microkernel hard),
but the success and simplicity of the thread handling code in this enticed
me to begin the full rewrite that Acess required.<br />
Acess2 was a complete rewrite of the entire kernel, with less ugly hacks.
When I started on Acess2, I kept in mind the possibility of porting it to
other architectures, hence the platform specific code (memory management,
process switching and system init) is kept in a separate directory to the
main kernel code, and the build system is designed to handle objects for
multiple architectures existing at the same time.<br />
Over Acess2's development time, I have been working on maintaining a
consitent driver interface, and I try to keep this well documented. There
is now driver specifcations for video, network and keyboards. With audio
on the way. For more information on these, see the documentation section.
{include "footer.tpl"}

