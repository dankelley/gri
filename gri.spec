Summary: A language for scientific illustration
%define griversion 2.10.1
Name: gri
Version: %{griversion}
Release: 1
Copyright: distributable
Group: Applications/Engineering
Source: ftp://ftp.phys.ocean.dal.ca/users/kelley/gri/gri-%{griversion}.tgz
URL: http://gri.sourceforge.net
Packager: Dan E. Kelley <Dan.Kelley@Dal.CA>
Icon: grilogo.gif
BuildRoot: /var/tmp/%{name}-root

%description
Gri is a language for scientific graphics programming.  It is a
command-driven application, as opposed to a click/point application.
It is analogous to latex, and shares the property that extensive power
is the reward for tolerating a modest learning curve.  Gri output is
in industry-standard PostScript, suitable for incorporation in
documents prepared by various text processors.

Gri can make x-y graphs, contour-graphs, and image graphs.  In
addition to high-level capabilities, it has enough low-level
capabilities to allow users to achieve a high degree of customization.
Precise control is extended to all aspects of drawing, including
line-widths, colors, and fonts.  Text includes a subset of the tex
language, so that it is easy to incorporate Greek letters and
mathematical symbols in labels.

The following is a terse yet working Gri program.  If it is stored in
a file called 'example.gri', and executed with the unix command 'gri
example', it will create a postscript file called 'example.ps' with
a linegraph connecting data points in the file called `file.dat'.

   open file.dat        # open a file with columnar data
   read columns x * y   # read first column as x and third as y
   draw curve           # draw line through data (autoscaled axes)

%prep
%setup -q

%build
%configure
make libdir=/usr/share/gri gri
strip gri

%install
# remove docinst if it is there from a previous build
rm -rf docinst
(cd doc ; make linux_redhat)
(cd doc ; make card-install-redhat CARD_DIR=..)
(cd doc ; make html-install HTML_DIR=../docinst/html EXAMPLES_DIR=../docinst/examples)
(cd docinst/html; ln -sf index.html gri1.html)
cp -f README-linux-redhat README || :
make DESTDIR=$RPM_BUILD_ROOT install_linux_redhat

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README copyright.txt refcard.ps cmdrefcard.ps
%doc docinst/html
/usr/bin/gri
/usr/bin/gri_unpage
/usr/bin/gri_merge
/usr/share/gri
/usr/share/man/man1/gri.*
/usr/share/man/man1/gri_unpage.*
/usr/share/man/man1/gri_merge.*
/usr/share/info/gri.info*
/usr/share/emacs/site-lisp/gri-mode.el

%post
# add a gri entry to /usr/share/info/dir
if [ "$1" = 1 ]
then
	if ! grep -i gri /usr/share/info/dir > /dev/null
	then
		/sbin/install-info --dir-file="/usr/share/info/dir" /usr/share/info/gri.info.gz
#		chmod a+r /usr/share/info/dir
	fi
fi

%postun
# remove instances of gri in /usr/share/info/dir
if [ "$1" = 0 ];
then
	if grep -i "gri" /usr/share/info/dir > /dev/null
	then
		/sbin/install-info --dir-file="/usr/share/info/dir" --remove /usr/share/info/gri.info.gz
#		grep -vi "gri" /usr/share/info/dir > /usr/info/share/dir.tmp
#		mv /usr/share/info/dir.tmp /usr/share/info/dir
#		chmod a+r /usr/share/info/dir
	fi
fi

%changelog
* Sat Jun 1 2002  Dan Kelley <Dan.Kelley@Dal.Ca>
- Version 2.10.1
- Fix Sourceforge bug 562911 (won't build with gcc-3.0)
- Fix Sourceforge bug 562558 ('draw title' confusion with log axes)
- Fix Sourceforge bug 562014 (won't build if popt library is unavailable)
- Fix SourceForge bug 558463 (in HTML docs, the ``press'' margin tag was misdirected)
- Fix SourceForge bug 562017 (parser fails with DOS end-of-line)
- Fix SourceForge bug 562017 ('new page' postscript error in gv viewer)

* Tue May 07 2002  Dan Kelley <Dan.Kelley@Dal.Ca>
- Version 2.10.0

* Sat Apr 20 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix Sourceforge bug #546109 (bounding box wrong if postscript clipping used)

* Mon Mar 18 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Permit 'draw label' coordinates in pt

* Sat Mar 16 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix Sourceforge bug #508657 (missing backslash in drawing undefined synonyms)
- Fix Sourceforge bug #482120 ('regress' ignores data weights)

* Tue Mar 12 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Permit 'draw box' to have coordinates in pt, as well as cm.
- Permit 'draw symbol' to have coordinates in pt, as well as cm.
- Permit 'draw line from' to have coordinates in pt, as well as cm.

* Wed Feb 27 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix Sourceforge bug #523450 (log axes detect non-positive values too late)

* Thu Feb 21 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix Sourceforge bug #521045 (install problem, function prototype problem)

* Thu Feb 07 2002  Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix Sourceforge bug #513002 (minor error in documentation of 'set clip').

* Mon Jan 28 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix Sourceforge bug #509592 (doc HTML indices misordered).

* Sat Jan 26 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix SourceForge bug #506523 (map axes give wrong minutes in negative regions).

* Fri Jan 25 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix SourceForge bug #508088 (grimode: gv should update, not be relaunched).
- Make RPM install/uninstall run silently.

* Wed Jan 23 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- Fix SourceForge bug #506490 ('-v' commandline option gave wrong number)

* Wed Jan 02 2002 Dan Kelley <Dan.Kelley@Dal.Ca>
- add `set clip to curve'

* Thu Dec 13 2001 Dan Kelley <Dan.Kelley@Dal.Ca>
- Release as gri-2.8.5 on SourceForge.Net site.  
- Fix SourceForge bug #492472 ('inf' rpn operator caused segfault)

* Thu Oct 04 2001 Dan Kelley <Dan.Kelley@Dal.Ca>
- Release as gri-2.8.4 on SourceForge.Net site.  
- Fix SourceForge bug #467973 (`gri -version' gave wrong version
  number, breaking the Emacs Gri mode.)
- Fix SourceForge bug #468401 (`draw grid' disobeys pencolor)

* Mon Oct 01 2001 Dan Kelley <Dan.Kelley@Dal.Ca>
- Release as gri-2.8.3 on SourceForge.Net site.
- Fix SourceForge bug #462243 (endian problem in Rasterfile images, 
  plus a reading problem in PGM images).

* Mon Sep 10 2001 Dan Kelley <Dan.Kelley@Dal.Ca>
- Release as gri-2.8.2 on SourceForge.Net site.
- Really Fix SourceForge bug #454557 (wouldn't compile with the
  pre-release version 3.0.1 of the GNU c++ compiler). 
  This closes SourceForge Bug #111093.

* Thu Sep 06 2001 Dan Kelley <Dan.Kelley@Dal.Ca>
- Release as gri-2.8.1 on SourceForge.Net site.
- Fix SourceForge bug #450465 (`create columns from function' was broken).
- Fix SourceForge bug #454557 (wouldn't compile with the pre-release 
  version 3.0.1 of the GNU c++ compiler; closes: #111093)

* Tue Jul 24 2001 Dan Kelley <dan.kelley@dal.ca>
- Bump up version number to 2.8.0

* Mon Jul 23 2001 Dan Kelley <Dan.Kelley@Dal.Ca>
- Release as gri-2.8.0 on SourceForge.Net site.
- Add `unlink' command as a unix-familiar way to delete files.
- Add `set page size' command to clip to a given page size.
- Add `substr' RPN operator to permit extraction of sub-strings.
- Add `default' for the `set x name' and the `set y name' commands.
- Add Perl-like ability to put underscores in numerical constants
    (`.v. = 1_000' and `.v. = 1000' are completely equivalent).
- In Emacs mode, change <M-Tab> so that it completes builtin 
  variables and synonyms as well as commands. 
- In Emacs mode, add "idle-timer help" to display defaults
  for builtin variables under cursor.
- In Emacs mode, make fontification of builtin variables apply 
  only if spelled correctly.
- To Makefile, add `make source-arch-indep' target in sources.  
  This will build a source tar file in which all the 
  architecture-independent material (documentation in HTML, 
  postscript and Info formats) is pre-made.  This makes it 
  easier to install gri on a host that doesn't have  TeX and
  ImageMagick installed.
- Move gri-html-doc and gri-ps-doc documentation files to 
  the /usr/share/doc/gri directory
- Ensure that package compiles with Standards-Version: 3.5.5
  without changes.

* Thu Apr 19 2001 Dan Kelley <dan.kelley@dal.ca>
- Rename this file as gri.spec, without the version number embedded 
  in the filename.  Upgrade to version number 2.6.1.  Change url to
  point to sourceforge site (but leave ftp as it is, for now anyway).

* Tue Jan 30 2001  Dan Kelley <dan.kelley@dal.ca>
- Changing to e.g. /usr/share/info instead of /usr/info.  Same for 
  manpages.  I know, I should be using the fancy macros that are 
  defined in /usr/lib/rpm, but these seemed contradictory, with 
  respect to where things are in my Redhat 7.0 setup ... and I
  had a hard time figuring out how to use these macros anyway,
  so I just gave up and hard-wired them in, using the new 
  directories as used in Redhat 7.0, as opposed to the (different)
  directories in all the other Redhat versions I've had.  Someday
  I'll switch to using macros, but it means changing both this 
  spec-file and various Makefiles, and I need to be sure that 
  changes to the Makefiles don't hurt the distributions for 
  Debian linux, for solaris, etc.

* Thu Jun  1 2000  Dan Kelley <dan.kelley@dal.ca>
- Triv changes here; code changes are to read compressed files, and
  manual improvements.

* Fri May 12 2000 Dan Kelley <dan.kelley@dal.ca>
- Compress info files for linux-redhat.

* Thu May 11 2000 Peter S Galbraith <psg@debian.org>
- Change info files to .info file extension.
- Tweaked install-info rules.  I hope they work.

* Sat Apr 01 2000 Dan Kelley <dan.kelley@dal.ca>
- Fix spec-file error in the install-info command.  However,
  to my great frustration, this is still broken or install-info
  is broken) since the command doesn't install an entry for gri.
  After hand-editing to insert a Gri entry, I uncovered another
  bug, and so I have added a chmod of /usr/info/dir file so 
  folks other than root can use info.
- Update the version number in gri.cmd to match the number compiled 
  into gri.
- Update the startup message from the old form to the new form.
- Call this release 3 to match Tim Powers' convention (although 
  I think it should be called release 1, when it works!)

* Fri Mar 31 2000 Dan Kelley <dan.kelley@dal.ca>
- applied Tim Powers' patches directly to the sources, updating them
  so that the patches Tim had made in this spec file are no longer
  needed.  Note: I didn't apply Tim's patch to the documentation, 
  since visual inspection indicated that I had already repaired
  the errors he found (each of which which involved my having used
  an incorrect name for the example gif files.)
- renamed Tim's spec file from gri.spec to gri-2.4.3.spec since
  otherwise I'd get too confused as versions develop.

* Fri Mar 31 2000 Tim Powers <timp@redhat.com>
- changed group

* Thu Mar 30 2000 Tim Powers <timp@redhat.com>
- started changelog on Dan Kelley's origional spec file
- quiet setup
- patched Makefile so that the install goes a bit smoother since we use
  BuildRoots
- changed post and postun sections so that they operate on /usr/info/dir
  instead of /etc/info-dir
- streamlined files list so that man/info pages are picked up even if RPM
  doesn't want to gzip them
- bzipped source to conserve space
- added clean section 
