<?php
require("subroutines.php");
set_up_navigation_tabs("download");
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
<!--
- <A HREF="<?php print "$ftp_site/gri-$gri_stable_version-arch-indep.tgz";		?>">w/ docs prebuilt	</a><br>
-->
- <A HREF="<?php print "$sf_url/cvs/?group_id=$gri_group_id";				?>">CVS  		</a><br>
<br>
<?php set_up_rhs(); ?>

<h1>Pre-compiled Packages</h1>
<p>
Packages are provided for <b>RedHat Linux</b>, in 
both
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.i386.rpm";?>">
binary
</a>
and
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-1.src.rpm";?>">
source
</a>
forms.


<p> Packages are available for
<b>
<a href="http://packages.debian.org/unstable/science/gri.html">
GNU/Debian linux</a></b>, in three forms:
(a) an <A HREF="<?php print "$ftp_site/gri_2.8.6-0potato1_i386.deb";?>">
old package</a>
suitable for the Potato distribution,
(b) an
<A HREF="<?php print "$ftp_site/gri_$gri_stable_version-0woody1_i386.deb";?>">
up-to-date package
</a>
for the woody distribution, and (c) an up-to-date
<A HREF="<?php print "$ftp_site/gri-${gri_stable_version}static_${gri_stable_version}-1_i386.deb";?>">
unofficial statically-linked package</a>.  (The latter is set up for
	installation alongside other Gri releases, and it
	therefore lacks Info files.)
Under GNU/Debian
the gri-mode has a package of its own, 
called
<a href="http://packages.debian.org/unstable/science/gri-el.html">gri-el</a>.<br>
</p>

<p>For <b>Macintosh OSX</b>, a 
<A HREF="http://fink.sourceforge.net/pdb/package.php/gri">Fink</a>  package
is available, and a OS-X bundle will be prepared given
sufficient interest.</p>

<p>Upon request, versions may be provided 
for <b>other platforms</b>, e.g. Solaris, Windows, etc.</p>

<h1>Source Code</h1>
<p>
The Gri source code is provided in two tarballs: a 
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version.tgz";?>">smaller package</a>
for which documentation files are created during the building process (but which
requires more software on the build machine), and a
<A HREF="<?php print "$ftp_site/gri-$gri_stable_version-arch-indep.tgz";?>">larger package</a>
which has pre-built documentation files.
</p>

<p> If you're adventurous and/or interested in helping with Gri development,
you might want to try building the
<A HREF="<?php print "$sf_url/cvs/?group_id=$gri_group_id";?>">CVS version</a>,
   which is the very latest version.  Please note, however, that this may not even compile; 
the developer's sandbox can be messy!
</p>

<?php
footer();
?>
