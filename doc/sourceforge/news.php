<?php
require("subroutines.php");
set_up_navigation_tabs("news", "news.png");
set_up_lhs();
?>
- <A href="<?php print "$sf_url/news/?group_id=$gri_group_id";?>">news forum</b><br>
<br><br>
- <A HREF="<?php print "$development_site/gridoc/html/Version_2_10.html#Version2.10";?>">recent changes<br>
<br><br>
- <A HREF="<?php print "$development_site/Plans.html";?>">plans<br>
<?php set_up_rhs(); ?>

<h1>Status</h1>

<p>
Stable version 
<?php printf(" %s released %s ago.", "$gri_stable_version", age()); ?>

<h1>News forum</h1>
<p>
A forum for Gri
<?php
print "<A href=\"$sf_url/news/?group_id=$gri_group_id\">\n";
?>
news
</a>
is available.

<h1>New features</h1>
<p>
<?php
print "<A HREF=\"$development_site/gridoc/html/Version_2_10.html#Version2.10\">\n";
?>
Changes
</a>
to Gri are documented in the manual.


<h1>Plans for next stable version</h1>

<p>
A sketch of the
<?php
print "<A HREF=\"$development_site/Plans.html\">";
?>
plans
</a> for upcoming versions is available.

<?php footer(); ?>
