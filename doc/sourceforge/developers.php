<?php
require("subroutines.php");
set_up_navigation_tabs("developers", "developers.png");
set_up_lhs();
set_up_rhs();
?>

<h1>Developers</h1>

<p>
<a href="http://sourceforge.net/users/dankelley/">
Dan Kelley
</a> wrote Gri, and
<a href="http://sourceforge.net/users/psg/">
Peter Galbraith
</a> wrote the Gri Emacs mode.  The two have collaborated on Gri development 
for a decade, and have enjoyed the help and advice of many users, named
in the Gri manual.

</p>


<h1>Development site</h1>

<p>
Gri  development is hosted on the 
<a href=<?php print "\"$development_site\">$development_site_name";?>
</a> site.


<h1>Quick links for developers</h1>

<p>
<A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16976">Developer forum</A><br>
<A HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">Feature requests</A><br>
<A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16974">Open forum</A><br>
<A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16975">Help forum</A><br>
<A HREF="http://sourceforge.net/tracker/?atid=105511&group_id=5511&func=browse">Bug reports</A><br>
<A HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">Feature requests</A><br>
<A HREF="http://www.sourceforge.net/cvs/?group_id=5511">Get CVS source</a><br>
<A HREF="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gri/gri/">View CVS history</a><br>


<?php
footer();
?>
