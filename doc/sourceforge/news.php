<?php
require("subroutines.php");
set_up_navigation_tabs("news", "news.png");
set_up_lhs();
print "- <A href=\"http://www.sourceforge.net/news/?group_id=5511\">news forum</b><br>\n";
print "- <A HREF=\"http://gri.sourceforge.net/gridoc/html/Version_2_10.html#Version2.10\">recent changes<br>\n";
print "- <A HREF=\"http://gri.sourceforge.net/Plans.html\">plans<br>\n";
set_up_rhs();
?>

<h1>Status</h1>

<p>
Stable version 
<?php printf(" %s released %s ago.", "$gri_stable_version", age()); ?>

<h1>News forum</h1>
<p>
A forum for Gri
<A href="http://www.sourceforge.net/news/?group_id=5511">
news
</a>
is available.

<h1>New features</h1>
<p>
<A HREF="http://gri.sourceforge.net/gridoc/html/Version_2_10.html#Version2.10">
Changes
</a>
to Gri are documented in the manual.


<h1>Plans for next stable version</h1>

<p>
A sketch of the
<A HREF="http://gri.sourceforge.net/Plans.html">
plans
</a> for upcoming versions is available.

<?php footer(); ?>
