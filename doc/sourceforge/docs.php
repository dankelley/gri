<?php
require("subroutines.php");
set_up_navigation_tabs("docs", "docs.png");
set_up_lhs();
print "<b>Literature</b><br>\n";
print "- <A HREF=\"http://www2.linuxjournal.com/lj-issues/issue75/3743.html\">Linux Journal</a><br>\n";
print "<br>\n";
print "<b>Gri Manual</b><br>\n";
print "- <A HREF=\"./gridoc/html/index.html\">HTML</A><br>\n";
print "- <A HREF=\"./gri.pdf\">PDF</A><br>\n";
print "<br>\n";
print "<b>Other</b><br>\n";
print "- <A HREF=\"./gridoc/html/FAQ.html\">FAQ</A><br>\n";
print "- <A HREF=\"./gri-cookbook/index.html\">cookbook</A><br>\n";
print "- <A HREF=\"./refcard.pdf\">main refcard</A><br>\n";
print "- <A HREF=\"./cmdrefcard.pdf\">cmd refcard</A><br>\n";
set_up_rhs();
?>

<h1>Publications</h1>

<p>
An 
<A HREF="http://www2.linuxjournal.com/lj-issues/issue75/3743.html">
article on Gri</a>
appeared in Linux Journal (July 2000).  This might be a good introduction 
for many users.

<h1>Online resources</h1>
<p>

The Gri manual is online at Gri.SourceForge.Net in 
<A HREF="./gridoc/html/index.html">HTML</A> and
<A HREF="./gri.pdf">PDF</A> forms,
along with a
<A HREF="./gridoc/html/FAQ.html">FAQ</A>, a
<A HREF="./gri-cookbook/index.html">cookbook</A> showing Gri "recipes",
and two quick-reference cards (presenting an
<A HREF="./refcard.pdf">overview</A> and a list of Gri
<A HREF="./cmdrefcard.pdf">commands</A>).
<p>
The form below may be used to search the index of the gri manual.<br>
<center>
<form action="search_concept.php">
<input type=TEXT name=search_for size=30>
<input type=SUBMIT name="Submit" value="Submit"> 
</center>
</form>


<h1>Download Manual</h1>
<p>
<?php note("not available yet"); ?>

<?php
footer();
?>
