<?php
require("subroutines.php");
set_up_navigation_tabs("contact", "contact.png");
set_up_lhs();
print "<b>Forums</b><br>\n";
print "- <A HREF=\"http://www.sourceforge.net/forum/forum.php?forum_id=16974\">open</A><br>\n";
print "- <A HREF=\"http://www.sourceforge.net/forum/forum.php?forum_id=16975\">help</A><br>\n";
print "- <A HREF=\"http://www.sourceforge.net/forum/forum.php?forum_id=16976\">developer</A><br>\n";
print "<br>\n";
print "<b>Other</b><br>\n";
print "- <A HREF=\"http://sourceforge.net/survey/survey.php?group_id=5511&survey_id=10761\">survey</a><br>\n";
print "- <A HREF=\"http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse\">feature request</a><br>\n";
set_up_rhs();
?>


<h1>Discussion Forums</h1>

<p>
Most Gri users should subscribe to the 
<A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16974">open</A> forum
and the <A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16975">help</A>
forum.  If you're interested in where Gri is going (as an observer or as a helper),
you should visit the <A HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16976">developer</A> forum from time to time.
</p>

<h1>Survey</h1>

<p>
Please take the Gri <A
HREF=http://sourceforge.net/survey/survey.php?group_id=5511&survey_id=10761>survey</a>
so the author will have a better idea of how Gri should evolve.
</p>

<h1>Requests</h1>

<p>
You may request a new Gri feature 
<A HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">
here</A>.  If it's really important, you may also want to email
the <a href="developers.php">developers</a>.
You may also request Gri <a href="http://sourceforge.net/tracker/?group_id=5511&atid=205511">support</a>,
although it's better to report a bug, or to post on the help forum.
</p>

<?php
footer();
?>
