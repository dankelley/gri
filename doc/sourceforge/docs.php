<?php
require("subroutines.php");
set_up_navigation_tabs("docs");
set_up_lhs();
?>
<b>Literature</b><br>
- <A HREF="<?php print "http://www2.linuxjournal.com/lj-issues/issue75/3743.html"; ?>"> Linux Journal</a><br>
<br>
<b>Gri Manual</b><br>
- <A HREF="<?php print "./gridoc/html/index.html";	?>"> HTML		</A><br>
- <A HREF="<?php print "./gri.pdf";			?>"> PDF		</A><br>
<br>
<b>Reference Cards</b><br>
- <A HREF="<?php print "./refcard.pdf";			?>"> general		</A><br>
- <A HREF="<?php print "./cmdrefcard.pdf";		?>"> command list	</A><br>
<br>
<b>Other</b><br>
- <A HREF="<?php print "gridoc/html/FAQ.html";		?>"> FAQ		</A><br>
- <A HREF="<?php print "gri-cookbook/index.html";	?>"> cookbook		</A><br>

<?php set_up_rhs();?>

<h1>Publications</h1>

<p>
Dan and Peter have written an
<A HREF="http://www2.linuxjournal.com/lj-issues/issue75/3743.html">
article on Gri</a>
in Linux Journal (July 2000); users may find this introduction gentler
than the manuals.
</p>


<h1>Online resources</h1>

<p>
The Gri manual is online at the
<?php print "$development_site_name ";?>
website, in 
<A HREF="./gridoc/html/index.html">HTML</A> and
<A HREF="./gri.pdf">PDF</A> forms.
Quick-reference cards present an
<A HREF="./refcard.pdf">overview</A> and a list of Gri
<A HREF="./cmdrefcard.pdf">commands</A>.
</p>

<p>
If you have questions you should consult the
<A HREF="./gridoc/html/FAQ.html">FAQ</A>, 
and if you'd like to see some real-life examples, consult the 
<A HREF="./gri-cookbook/index.html">cookbook</A>.
</p>

<p>
The form below may be used to search the index of the gri manual.<br>
<center>
<form action="search_concept.php">
<input type=TEXT name=search_for size=30>
<input type=SUBMIT name="Submit" value="Submit"> 
</form>
</center>

<!--
<h1>Download Manual</h1>
<p>
<?php note("not available yet"); ?>
-->

<?php
footer();
?>
