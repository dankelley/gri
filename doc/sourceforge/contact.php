<?php
require("subroutines.php");
set_up_navigation_tabs("contact", "contact.png");
set_up_lhs();
print "<b>Forums</b><br>\n";
print "- <A HREF=\"$sf_url/forum/forum.php?forum_id=16974\">open</A><br>\n";
print "- <A HREF=\"$sf_url/forum/forum.php?forum_id=16975\">help</A><br>\n";
print "- <A HREF=\"$sf_url/forum/forum.php?forum_id=16976\">developer</A><br>\n";
print "<br>\n";
print "<b>Other</b><br>\n";
print "- <A HREF=\"$sf_url/survey/survey.php?group_id=$gri_group_id&survey_id=10761\">survey</a><br>\n";
print "- <A HREF=\"$sf_url/tracker/?atid=35$gri_group_id&group_id=$gri_group_id&func=browse\">feature request</a><br>\n";
set_up_rhs();
?>


<h1>Discussion Forums</h1>

<p>
Most Gri users should subscribe to the 
<?php
print "<A HREF=\"$sf_url/forum/forum.php?forum_id=16974\">";
?>
open</A> forum
and the
<?php 
print "<A HREF=\"$sf_url/forum/forum.php?forum_id=16975\">";
?>
help</A>
forum.  If you're interested in where Gri is going (as an observer or as a helper),
you should visit the
<?php
print "<A HREF=\"$sf_url/forum/forum.php?forum_id=16976\">\n";
?>
developer</A> forum from time to time.
</p>

<h1>Survey</h1>

<p>
Please take the Gri 
<?php
print "<A HREF=\"$sf_url/survey/survey.php?group_id=$gri_group_id&survey_id=10761\">\n";
?>
survey</a> so the author will have a better idea of how Gri should evolve.
</p>

<h1>Requests</h1>

<p>
You may request a new Gri feature 
<?php
print "<A HREF=\"$sf_url/tracker/?atid=35$gri_group_id&group_id=$gri_group_id&func=browse\">\n";
?>
here</A>.  If it's really important, you may also want to email
the <a href="developers.php">developers</a>.
You may also request 
Gri
<?php
print "<a href=\"$sf_url/tracker/?group_id=$gri_group_id&atid=20$gri_group_id\">\n";
?>
support</a>,
although it's better to report a bug, or to post on the help forum.
</p>

<?php
footer();
?>
