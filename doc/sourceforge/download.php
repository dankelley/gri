<?php
require("subroutines.php");
set_up_navigation_tabs("download", "download.png");
set_up_lhs();
print "<b>Redhat Linux</b><br>\n";
print "- <A HREF=\"$ftp_site/gri-$gri_stable_version-1.src.rpm\">binary</a><br>\n";
print "- <A HREF=\"$ftp_site/gri-$gri_stable_version-1.i386.rpm\">source</a><br>\n";
print "<br>\n";
print "<b>Debian/GNU Linux</b><br>\n";
print "- <A HREF=\"$ftp_site/gri_2.8.6-0potato1_i386.deb\">potato [old]</a><br>\n";
print "- <A HREF=\"$ftp_site/gri_$gri_stable_version-1_i386.deb\">woody</a><br>\n";
print "- <A HREF=\"$ftp_site/gri-${gri_stable_version}static_2.10.1-1_i386.deb\">static</a><br>\n";
print "<br>\n";
print "<b>Other OS</b><br>\n";
print "- <A HREF=\"$ftp_site/gri-$gri_stable_version-SunOS5.tar.gz\">SunOS</a><br>\n";
print "- <A HREF=\"http://gnuwin32.sourceforge.net/packages/gri.htm\">Windows</a><br>\n";
print "- <A HREF=\"http://fink.sourceforge.net/pdb/package.php/gri\">Apple OS-X</a><br>\n";
print "<br>\n";
print "<br>\n";
print "<b>Any OS/version</b><br>\n";
print "- <A HREF=\"$sf_url/project/showfiles.php?group_id=$gri_group_id\">list</a>\n";
print "<br>\n";
print "<br>\n";
print "<br>\n";
print "<b>Source Code</b><br>\n";
print "- <A HREF=\"$ftp_site/gri-$gri_stable_version.tgz\">tarball</a><br>\n";
print "- <A HREF=\"$ftp_site/gri-$gri_stable_version-arch-indep.tar.gz\">\" w/ docs prebuilt</a>\n";
set_up_rhs();
?>

<h1>Pre-compiled Packages</h1>
<p>
For <b>RedHat Linux</b>,
both <?php print "<A HREF=\"$ftp_site/gri-$gri_stable_version-1.src.rpm\">binary</a>\n"; ?>
and
<?php print "<A HREF=\"$ftp_site/gri-$gri_stable_version-1.i386.rpm\">source</a>\n"; ?>
RPM packages are available.

<p> For <b>GNU/Debian linux</b>, three packages are available:
an
<?php
print "<A HREF=\"$ftp_site/gri_2.8.6-0potato1_i386.deb\">old package</a> for the Potato distribution, a newer package for the \n";
print "<A HREF=\"$ftp_site/gri_$gri_stable_version-1_i386.deb\">woody</a> distribution, and\n";
print "a <A HREF=\"$ftp_site/gri-${gri_stable_version}static_2.10.1-1_i386.deb\">static</a> package, with no documentation files.</a>\n";
?>
The Potato package is the last version of gri which would build without
changes in that distribution (due to uncompatible changes in debhelper
scripts which are used to make the package).  The woody package is
identical to the package currently in Debian's unstable distribution (sid),
but is woody-compatible with respect to libraries.  The static package
is a version-specific package made to live alongside other releases
(possibly newer) of the regular gri package, thus providing backwards
compatibility for your old code that may depend on a version of Gri.  To
distinguish this package from the regular `gri' package, it is named as
<i>gri-2.10.1static</i> as opposed to simply <i>gri</i>.  It does not
contain Info files or the Emacs gri-mode;  install a regular version of the
Gri package to get those.

<p>For <b>Solaris OS5</b>,
a <?php print "<A HREF=\"$ftp_site/gri-$gri_stable_version-SunOS5.tar.gz\">pre-compiled tarball</a>\n"; ?>
is available.

<p>Pre-compiled packages are also available for
<A HREF="http://gnuwin32.sourceforge.net/packages/gri.htm">windows</a>
and for 
<A HREF="http://fink.sourceforge.net/pdb/package.php/gri">Macintosh OS-X</a>.


<p>
Visit the
<?php print $development_site_name;?> site for full list of
<a href="$sf_url/project/showfiles.php?group_id=$gri_group_id">
of downloadable files</a>.

<h1>Source Code</h1>
<p>
The Gri source code is provided in two tarballs: a 
<?php print "<A HREF=\"$ftp_site/gri-$gri_stable_version.tgz\">smaller package</a>\n"; ?>
for which documentation files are created during the building process, and a
<?php print "<A HREF=\"$ftp_site/gri-$gri_stable_version-arch-indep.tar.gz\">larger package</a>\n"; ?>
which has pre-built documentation files.
</p>

<?php
footer();
?>
