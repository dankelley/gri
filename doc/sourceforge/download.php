<?php
require("subroutines.php");
set_up_navigation_tabs("download");
set_up_lhs();
?>
<b>Debian/GNU Linux</b><br>
<!--
- <A HREF="<?php print "$ftp_site/gri_2.8.6-0potato1_i386.deb";?>">potato [old]</a><br>
- <A HREF="<?php print "$ftp_site/gri_$gri_stable_version-0woody1_i386.deb";?>">woody</a><br>
- <A HREF="<?php print "$ftp_site/gri-${gri_stable_version}static_${gri_stable_version}-1_i386.deb";?>">statically linked</a><br>
-->
- <A HREF="http://packages.debian.org/unstable/science/gri.html">Gri
at Debian</a><br>
- <a href="http://packages.debian.org/unstable/science/gri-el.html">Emacs mode</a><br>
- <A HREF="http://packages.debian.org/unstable/doc/gri-html-doc.html">HTML manual</a><br>
- <A HREF="http://packages.debian.org/unstable/doc/gri-ps-doc.html">PostScript
manual</a><br>
<br>
<b>OSX</b><br>
- <A HREF="<?php print "http://fink.sourceforge.net/pdb/package.php/gri";?>">Fink</a><br>
- <A HREF="<?php print "http://www.macports.org/";?>">MacPorts</a><br>
<br><br>
<b>Any OS/version</b><br>
- <A HREF="<?php print "$sf_url/project/showfiles.php?group_id=$gri_group_id";		?>">list		</a><br>
<br>
<?php set_up_rhs(); ?>


<h1>Packages</h1>

<p> Pre-compiled packages are available for
<a href="http://packages.debian.org/unstable/science/gri.html">
GNU/Debian linux</a>,
for Macintosh (in <A HREF="http://fink.sourceforge.net/pdb/package.php/gri">fink</a>
and <A HREF="http://www.darwinports.org/">DarwinPort</A> forms), 
and for MSWindows
(in <a href="http://gnuwin32.sourceforge.net/packages/gri.htm">
gnuwin32</a> form).  Some old versions have been packaged for Redhat; see
the 
<?php print "<a href=\"$sf_url/project/showfiles.php?group_id=$gri_group_id\">sourceforge repository</a>";?>
 for details.</p>

<h1>Source Code</h1>

<p> A tarball at the
<?php print "<a href=\"$sf_url/project/showfiles.php?group_id=$gri_group_id\">sourceforge repository</a>";?>
 provides the Gri source in a standard way, i.e. with the usual steps of <tt>./configure</tt> followed by <tt>make</tt> and
then <tt>make install</tt>. (Other standard targets exist also, such as <tt>make check</tt>, etc.) </p>

<p> If you're adventurous and/or interested in helping with Gri development,
you might want to try building the
<A HREF="<?php print "$sf_url/cvs/?group_id=$gri_group_id";?>">CVS version</a>,
   which is the very latest version.  Please note, however, that this may not even compile; 
a developer's sandbox can be messy!
</p>

<?php
footer();
?>
