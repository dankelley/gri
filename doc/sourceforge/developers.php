<?php
require("subroutines.php");
set_up_navigation_tabs("developers");
set_up_lhs();
set_up_rhs();
?>

<h1>Developers</h1>

<p> <a href="<?php print "$sf_url/users/dankelley/";?>">Dan Kelley</a>
wrote Gri, and <a href="<?php print "$sf_url/users/psg/";?>">Peter
Galbraith</a> wrote the Gri Emacs mode.  The two have collaborated on
Gri development (and science) for nearly two decades, and have enjoyed
the help and advice of many users, named in the Gri manual.

</p>


<h1>Development site</h1>

<p>
Gri  development was hosted on SourceForge.net until April 2010, after
which it was hosted at 
<a href="http://github.com/dankelley/gri">GitHub</a>.


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
print "Trackers (legacy): ";
print "[<A HREF=\"$sf_url/tracker/?atid=35$gri_group_id&amp;group_id=$gri_group_id&amp;func=browse\">Feature requests</A>] ";
print "[<A HREF=\"$sf_url/tracker/?atid=10$gri_group_id&amp;group_id=$gri_group_id&amp;func=browse\">Bug reports</A>] ";
print "[<A HREF=\"$sf_url/tracker/?atid=30$gri_group_id&amp;group_id=$gri_group_id&amp;func=browse\">Patch submissions</A>]<br>\n";
print "Trackers (present-day): ";
print "[<A HREF=\"http://github.com/dankelley/gri/issues\">issues</a>]<br>\n";
print "<h1>Developer Work Cycle</h1>\n";
print "[<A HREF=\"http://github.com/dankelley/gri\">Git summary</a>]<br>";
print "Do this one time:<br>";
print "<tt>git clone git://github.com/dankelley/gri.git</tt></p>\n";
print "Work cycle:<br>\n";
print "<tt>git pull</tt> # get updates from origin<br>\n";
print "<tt>emacs ...</tt> # do some work<br>\n";
print "<tt>git -am 'commit message'</tt> # can also do add then commit<br>\n";
print "<tt>git push</tt> # push your own updates back to origin<br>\n";
?>

<?php
footer();
?>
