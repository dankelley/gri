<?php
require("subroutines.php");
set_up_navigation_tabs("bugs", "bugs.png");
set_up_lhs();
print "- <A HREF=\"$sf_url/tracker/?atid=105511&group_id=$gri_group_id&func=browse\">bug browser</A><br><br><br>\n";
print "- <A HREF=\"$sf_url/tracker/?atid=105511&group_id=$gri_group_id&func=add&group_id=$gri_group_id&atid=105511\">report a bug</A><br>\n";
set_up_rhs();
?>

<h1>Known bugs</h1>
<p>
The details of
<?php print "<A HREF=\"$sf_url/tracker/?atid=105511&group_id=$gri_group_id&func=browse\">";?>
known bugs</A> may be browsed online.  Be sure to check both open and closed bugs.
</p>

<h1>Report a bug</h1>

<p>
Please
<?php print "<A HREF=\"$sf_url/tracker/?atid=105511&group_id=$gri_group_id&func=add&group_id=$gri_group_id&atid=105511\">"; ?>
report a bug</A> if you find one, to help yourself and to help other Gri users.
</p>

<?php
footer();
?>
