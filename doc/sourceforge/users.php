<?php
require("subroutines.php");
set_up_navigation_tabs("discussion", "discussion.png");
set_up_lhs();
print "<b>Forums</b><br>\n";
print "- <A HREF=\"http://www.sourceforge.net/forum/forum.php?forum_id=16974\">open</A><br>\n";
print "- <A HREF=\"http://www.sourceforge.net/forum/forum.php?forum_id=16975\">help</A><br>\n";
print "- <A HREF=\"http://www.sourceforge.net/forum/forum.php?forum_id=16976\">developer</A><br>\n";
print "<br>\n";
print "<b>Othe</b><br>\n";
print "- <A HREF=\"http://sourceforge.net/survey/survey.php?group_id=5511&survey_id=10761\">survey</a><br>\n";
print "- <A HREF=\"http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse\">feature request</a><br>\n";
set_up_rhs();
?>


<h1>Discussion</h1>
<p>
Most Gri users should subscribe to the 
<A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16974">open</A> forum
and the <A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16975">help</A>
forum.  If you're interested in where Gri is going (as an observer or as a helper),
you should visit the <A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16976">developer</A> forum from time to time.

<h1>Feature requests</h1>

<p>
You may request a new Gri feature 
<A HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">
here</A>.  If it's really important, you may also want to email the author directly.

<?php
footer();
?>
