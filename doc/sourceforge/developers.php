<?php
require("subroutines.php");
set_up_navigation_tabs("developers", "developers.png");
set_up_lhs();
set_up_rhs();
?>

<h1>Developers</h1>

<p>
<a href="<?php print "$sf_url/users/dankelley/";?>">Dan Kelley</a>
wrote Gri, and
<a href="<?php print "$sf_url/users/psg/";?>">Peter Galbraith</a>
wrote the Gri Emacs mode.  The two have collaborated on Gri development 
for a decade, and have enjoyed the help and advice of many users, named
in the Gri manual.

</p>


<h1>Development site</h1>

<p>
Gri  development is hosted on the 
<a href="<?php print "\"$development_site\">$development_site_name";?>
</a> site.


<h1>Quick links for developers</h1>

<p>
<?php
print "Forums: ";
print "<A HREF=\"$sf_url/forum/forum.php?forum_id=16976\">developer</A> ... \n";
print "<A HREF=\"$sf_url/forum/forum.php?forum_id=16974\">open</A> ... \n";
print "<A HREF=\"$sf_url/forum/forum.php?forum_id=16975\">help</A>.<br>\n";
print "<A HREF=\"$sf_url/tracker/?atid=35$gri_group_id&group_id=$gri_group_id&func=browse\">Feature requests</A><br>\n";
print "<A HREF=\"$sf_url/tracker/?atid=10$gri_group_id&group_id=$gri_group_id&func=browse\">Bug reports</A><br>\n";
print "<A HREF=\"$sf_url/tracker/?atid=30$gri_group_id&group_id=$gri_group_id&func=browse\">Patch submissions</A><br>\n";
print "<A HREF=\"$sf_url/pm/task.php?group_project_id=8706&group_id=$gri_group_id&func=browse\">View to-do list</A><br>\n";
print "<A HREF=\"$sf_url/cvs/?group_id=$gri_group_id\">Get CVS source</a><br>\n";
print "<A HREF=\"http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gri/gri/\">View CVS history</a><br>\n";
?>

<?php
footer();
?>
