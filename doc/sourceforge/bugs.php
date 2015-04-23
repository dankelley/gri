<?php
require("subroutines.php");
set_up_navigation_tabs("bugs");
set_up_lhs();
#print "- <A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=browse\">bug browser</A><br><br><br>\n";
#print "- <A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=add&amp;group_id=$gri_group_id&amp;atid=105511\">report a bug</A><br>\n";
set_up_rhs();
?>

<h1>Known bugs</h1>
<p>
The details of known bugs may be browsed online. Bugs reported prior to 2010-05-06 are reported on the
<?php print "<A HREF=\"$sf_url/tracker/?limit=25&func=&group_id=5511&atid=105511&assignee=&status=&category=&artgroup=&keyword=&submitter=&artifact_id=&assignee=&status=1&category=&artgroup=&submitter=&keyword=&artifact_id=&submit=Filter&mass_category=&mass_priority=&mass_resolution=&mass_assignee=&mass_artgroup=&mass_status=&mass_cannedresponse=\">";?>SourceForge website</A> while those reported afterwards are reported on the <a href=\"https://github.com/dankelley/gri/issues\">GitHub website</a>.
</p>

<h1>Report a bug</h1>

<p>
Please use the
<?php print "<A HREF=\"https://github.com/dankelley/gri/issues\">"; ?>
GitHub website</a> to report a bug if you find one, to help yourself and other Gri users.
</p>

<?php
footer();
?>
