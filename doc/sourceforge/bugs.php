<?php
require("subroutines.php");
set_up_navigation_tabs("bugs");
set_up_lhs();
print "- <A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=browse\">bug browser</A><br><br><br>\n";
print "- <A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=add&amp;group_id=$gri_group_id&amp;atid=105511\">report a bug</A><br>\n";
set_up_rhs();
?>

<h1>Known bugs</h1>
<p>
The details of
<?php print "<A HREF=\"$sf_url/tracker/?limit=25&func=&group_id=5511&atid=105511&assignee=&status=&category=&artgroup=&keyword=&submitter=&artifact_id=&assignee=&status=1&category=&artgroup=&submitter=&keyword=&artifact_id=&submit=Filter&mass_category=&mass_priority=&mass_resolution=&mass_assignee=&mass_artgroup=&mass_status=&mass_cannedresponse=\">";?>known bugs</A> may be browsed online.
</p>

<h1>Report a bug</h1>

<p>
Please
<?php print "<A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=add&amp;group_id=$gri_group_id&amp;atid=105511\">"; ?>
report a bug</A> if you find one, to help yourself and to help other Gri users.
</p>

<?php
footer();
?>
