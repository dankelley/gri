<?php
require("subroutines.php");
set_up_navigation_tabs("download", "download.png");
set_up_lhs();
?>
<b>Redhat Linux</b>
<br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.src.rpm";			?>">binary		</a><br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.i386.rpm";			?>">source		</a><br>
<br>
<b>Debian/GNU Linux</b><br>
- <A HREF="<?php print "$ftp_site/gri_2.8.6-0potato1_i386.deb";				?>">potato [old]	</a><br>
- <A HREF="<?php print "$ftp_site/gri_$gri_stable_version-1_i386.deb";			?>">woody		</a><br>
- <A HREF="<?php print "$ftp_site/gri-${gri_stable_version}static_2.10.1-1_i386.deb";	?>">static		</a><br>
<br>
<b>Other OS</b><br>
- <A HREF=<?php print "$ftp_site/gri-$gri_stable_version-SunOS5.tar.gz";		?>">SunOS		</a><br>
- <A HREF=<?php print "http://gnuwin32.sourceforge.net/packages/gri.htm";		?>">Windows		</a><br>
- <A HREF=<?php print "http://fink.sourceforge.net/pdb/package.php/gri";		?>">Apple OS-X		</a><br>
<br><br>
<b>Any OS/version</b><br>
- <A HREF="<?php print "$sf_url/project/showfiles.php?group_id=$gri_group_id";		?>">list		</a><br>
<br>
<b>Source Code</b>
<br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version.tgz";				?>">tarball		</a><br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-arch-indep.tar.gz";		?>">\" w/ docs prebuilt	</a><br>
<br><br>
<b>CVS</b><br>
- <A HREF="<?php print "$sf_url/cvs/?group_id=$gri_group_id";				?>">unpackaged		</a><br>
<br>
<?php set_up_rhs(); ?>

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
print "a <A HREF=\"$ftp_site/gri-${gri_stable_version}static_2.10.1-1_i386.deb\">static</a> package, with no documentation files.\n";
?>
The Potato package is the lastest gri version that compiles without
changes in that distribution (due to uncompatible changes in debhelper
scripts which are used to make the package).  The woody package is
identical to the package currently in Debian's unstable distribution (sid),
but is woody-compatible with respect to libraries.  The static package
is a version-specific package made to live alongside other releases
of the regular gri package, yielding backwards
compatibility.  To
distinguish this package from the regular `gri' package, it is named 
<i>gri-2.10.1static</i> as opposed to <i>gri</i>.  It does not
contain Info files or the Emacs gri-mode; install a regular version of the
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
downloadable files</a>.

<h1>Source Code</h1>
<p>
The Gri source code is provided in two tarballs: a 
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version.tgz";?>">smaller package</a>
for which documentation files are created during the building process, and a
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-arch-indep.tar.gz";?>">larger package</a>
which has pre-built documentation files.
</p>

<p> If you're adventurous and/or interested in helping with Gri developement,
you might want to try building the
<A HREF="<?php print "$sf_url/cvs/?group_id=$gri_group_id";?>">CVS version</a>,
which is the very latest version, and which may not even compile.
</p>

<?php
footer();
?>
