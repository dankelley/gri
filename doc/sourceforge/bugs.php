<?php
require("subroutines.php");
set_up_navigation_tabs("bugs", "bugs.png");
set_up_lhs();
print "- <A HREF=\"$sf_url/tracker/?atid=105511&group_id=$gri_group_id&func=browse\">bug browser</A><br>\n";
set_up_rhs();
?>

<p>
To browse bug reports, or to submit a bug report, go
<?php
print "<A HREF=\"$sf_url/tracker/?atid=105511&group_id=$gri_group_id&func=browse\">";
?>
here</A>.<br>

<?php
footer();
?>
