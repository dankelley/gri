<?php
require("subroutines.php");
set_up_navigation_tabs("news", "news.png");
set_up_lhs();
?>
- <A href="<?php print "$sf_url/news/?group_id=$gri_group_id";?>">news forum</a><br>
<br><br>
- <A HREF="<?php print "$development_site/gridoc/html/Version_2_12.html#Version2.12";?>">recent changes</a><br>
<br><br>
- <A HREF="<?php print "$development_site/Plans.html";?>">plans</a><br>
<?php set_up_rhs(); ?>

<h1>Status</h1>

<p>
Stable version 
<?php printf(" %s was released %s ago.", "$gri_stable_version", age()); ?>

<h1>News forum</h1>
<p>
A forum for Gri
<?php
print "<A href=\"$sf_url/news/?group_id=$gri_group_id\">\n";
?>
news
</a>
is available.

<h1>The Past and the Future</h1>
<p>
The online manual discusses
<?php
print "<A HREF=\"$development_site/gridoc/html/Version_${gri_stable_version_major}_${gri_stable_version_intermediate}.html#Version$gri_stable_version_major.$gri_stable_version_intermediate\">\n";
?>
recent changes
</a>
to Gri, as well as ideas for
<?php
print "<A HREF=\"./gridoc/html/Plans.html#Plans\">";
?>
upcoming features
</a> that are under consideration.
</p>

<?php footer(); ?>
