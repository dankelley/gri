<?php
require("subroutines.php");
set_up_navigation_tabs("developers");
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
<?php print "<a href=\"$sf_url/projects/gri/\">$development_site_name";?>
</a> site.


<h1>Quick links for developers</h1>

<p>
<?php
print "Forums: ";
print "[<A HREF=\"$sf_url/forum/forum.php?forum_id=16976\">developer</A>] ";
print "[<A HREF=\"$sf_url/forum/forum.php?forum_id=16974\">open</A>] ";
print "[<A HREF=\"$sf_url/forum/forum.php?forum_id=16975\">help</A>].<br>\n";
print "Debian: ";
print "[<a href=\"http://packages.qa.debian.org/g/gri.html\">package</a>]
       [<a href=\"http://buildd.debian.org/build.php?pkg=gri\">auto-builder's log</a>].\n";
print "<br>";
print "Trackers: ";
print "[<A HREF=\"$sf_url/tracker/?atid=35$gri_group_id&amp;group_id=$gri_group_id&amp;func=browse\">Feature requests</A>] ";
print "[<A HREF=\"$sf_url/tracker/?atid=10$gri_group_id&amp;group_id=$gri_group_id&amp;func=browse\">Bug reports</A>] ";
print "[<A HREF=\"$sf_url/tracker/?atid=30$gri_group_id&amp;group_id=$gri_group_id&amp;func=browse\">Patch submissions</A>]\n";
print "<br> Dan Kelley:";
print "[<a HREF=\"$sf_url/developer/diary.php?diary_user=31559\">Diary</a>]\n";
print "[<A HREF=\"$sf_url/pm/task.php?group_project_id=8706&amp;group_id=$gri_group_id&amp;func=browse\">To-do list</A>].\n";
print "<br>";
#print "CVS:\n";
#print "[<A HREF=\"$sf_url/cvs/?group_id=$gri_group_id\">Get CVS source</a>]\n";
#print "[<A HREF=\"http://gri.cvs.sourceforge.net/gri/gri/\">View CVS history</a>].\n";
print "<p>Git:\n";
print "[<A HREF=\"http://gri.git.sourceforge.net/git/gitweb.cgi?p=gri/gri;a=summary\">summary</a>]<br>";
print "Do this one time:<br>";
print "<tt>git clone ssh://dankelley@gri.git.sourceforge.net/gitroot/gri/gri</tt></p>\n";
print "Work cycle:<br>\n";
print "<tt>git pull</tt> # pull any updates from sourceforge<br>\n";
print "<tt>emacs ...</tt> # do some work<br>\n";
print "<tt>git -am 'commit message'</tt> # can also do add then commit<br>\n";
print "<tt>git push</tt> # push your own updates  back to sourceforge<br>\n";
?>

<?php
footer();
?>
