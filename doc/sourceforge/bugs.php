<?php
require("subroutines.php");
set_up_navigation_tabs("bugs", "bugs.png");
set_up_lhs();
print "- <A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=browse\">bug browser</A><br><br><br>\n";
print "- <A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;func=add&amp;group_id=$gri_group_id&amp;atid=105511\">report a bug</A><br>\n";
set_up_rhs();
?>

<h1>Known bugs</h1>
<p>
The details of
<?php print "<A HREF=\"$sf_url/tracker/?atid=105511&amp;group_id=$gri_group_id&amp;atid=105511\">";?>
known bugs</A> may be browsed online.  Bugs that apply have been fixed
in the CVS version, but not in the most recent release, are denoted
<b>pending</b>.  Bugs that have not yet been fixed are denoted
<b>open</b>.  Bugs that existed before the present release are denoted
<b>closed</b>.

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
