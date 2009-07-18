<?php
require("subroutines.php");
set_up_navigation_tabs("index");
set_up_lhs();
print "<font size=\"+2\">G r i</font><br><br><br><br>\n";
print "<font size=\"+2\">V e r s i o n</font><br><br><br><br>\n";
print "<font size=\"+2\">$gri_stable_version</font><br><br><br>\n";
set_up_rhs();
?>

<!--
<h1>Version</h1>
<p>
The present stable version is numbered <?php print $gri_stable_version; ?>.
-->

<h1>What Gri is</h1>

<p> Gri is a language for scientific graphics programming. The word
"language" is important: Gri is command-driven, not point/click.</p>

<p> Some users liken Gri to LaTeX, since both provide extensive power in
exchange for patience in learning syntax.</p>

<p>
Gri can make 
<a href="./gridoc/html/X-Y.html">
x-y
</a> graphs,
<a href="./gridoc/html/ContourPlots.html">
contour
</a> graphs, and 
<a href="./gridoc/html/Images.html">
image
</a>
graphs, in PostScript and (someday) SVG formats.
Control is provided over all aspects of drawing,
e.g. line widths, colors, and fonts.  A TeX-like syntax provides 
common mathematical symbols.</p>

<h1>Using Gri</h1>

<p>People often start using Gri by writing something like the following,
which produces an auto-scaled graph.<br>
<pre>
   open file.dat
   read columns x y
   draw curve
</pre>
<p>As they learn that everything is controllable in Gri, users
often start to produce complicated and specialized graphs.  For
example, every graph and table in
<a href="http://www.dfo-mpo.gc.ca/CSAS/Csas/Publications/ResDocs-DocRech/2009/2009_014_E.pdf">this report</a> was produced with Gri.  Writing complicated
code is made easier by the Gri emacs mode, which 
provides syntax colouring, command completion, and tight linkage to the online
manual; see the 
<a href="https://sourceforge.net/project/screenshots.php?group_id=5511">
screenshots</a> for illustration.</p>

<?php footer(); ?>
