<?php
require("subroutines.php");
set_up_navigation_tabs("download", "download.png");
set_up_lhs();
?>
<b>Redhat Linux</b>
<br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.i386.rpm";			?>">binary		</a><br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.src.rpm";			?>">source		</a><br>
<br>
<b>Debian/GNU Linux</b><br>
- <A HREF="<?php print "$ftp_site/gri_2.8.6-0potato1_i386.deb";				?>">potato [old]	</a><br>
- <A HREF="<?php print "$ftp_site/gri_$gri_stable_version-0woody1_i386.deb";			?>">woody		</a><br>
- <A HREF="<?php print "$ftp_site/gri-${gri_stable_version}static_${gri_stable_version}-1_i386.deb";
        ?>">statically linked		</a><br>
- <A HREF="http://packages.debian.org/unstable/science/gri.html">Gri
at Debian</a><br>
- <a href="http://packages.debian.org/unstable/science/gri-el.html">Emacs mode</a><br>
- <A HREF="http://packages.debian.org/unstable/doc/gri-html-doc.html">HTML manual</a><br>
- <A HREF="http://packages.debian.org/unstable/doc/gri-ps-doc.html">PostScript
manual</a><br>
<br>
<b>Other OS</b><br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-SunOS5.tar.gz";		?>">SunOS		</a><br>
- <A HREF="<?php print "http://gnuwin32.sourceforge.net/packages/gri.htm";		?>">Windows		</a><br>
- <A HREF="<?php print "http://fink.sourceforge.net/pdb/package.php/gri";		?>">Apple OS-X		</a><br>
<br><br>
<b>Any OS/version</b><br>
- <A HREF="<?php print "$sf_url/project/showfiles.php?group_id=$gri_group_id";		?>">list		</a><br>
<br>
<b>Source Code</b>
<br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version.tgz";				?>">tarball		</a><br>
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-arch-indep.tgz";		?>">w/ docs prebuilt	</a><br>
- <A HREF="<?php print "$sf_url/cvs/?group_id=$gri_group_id";				?>">CVS  		</a><br>
<br>
<?php set_up_rhs(); ?>

<h1>Pre-compiled Packages</h1>
<p>
For <b>RedHat Linux</b>,
both
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.i386.rpm";?>">
binary
</a>
and
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.src.rpm";?>">
source
</a>
RPM packages are available.

<p> For <b>GNU/Debian linux</b>, three packages are available:
a somewhat <A HREF="<?php print "$ftp_site/gri_2.8.6-0potato1_i386.deb";?>">
old package</a>
suitable for the Potato distribution,
along with two up-to-date packages:
<A HREF="<?php print "$ftp_site/gri_$gri_stable_version-0woody1_i386.deb";?>">
the
<?php print "${gri_stable_version}";?>
 package compiled for woody
</a>
, and
<A HREF="<?php print "$ftp_site/gri-${gri_stable_version}static_${gri_stable_version}-1_i386.deb";?>">
an unofficial statically linked package of
<?php print "${gri_stable_version}";?>
</a>.  The statically linked package is
for installation alongside other Gri releases, yielding
backwards compatibility, and therefore it lacks Info files.
<font color="green">
To distinguish this package from the regular gri package, it is named 
<i>
<?php print "gri-${gri_stable_version}static";?>
</i> as opposed to <i>gri</i>.
</font><br>
The Emacs gri-mode has recently been split off into it's own package
called
<a href="http://packages.debian.org/unstable/science/gri-el.html">gri-el</a>.<br>
Consult the
<a href="http://packages.debian.org/unstable/science/gri.html">
Gri page at Debian</a> for packages available for each Debian release as well
as documentation packages.
</p>

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
<a href="<?php print "$sf_url/project/showfiles.php?group_id=$gri_group_id";?>">
downloadable files</a>.

<h1>Source Code</h1>
<p>
The Gri source code is provided in two tarballs: a 
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version.tgz";?>">smaller package</a>
for which documentation files are created during the building process, and a
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-arch-indep.tgz";?>">larger package</a>
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
