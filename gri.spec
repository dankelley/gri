Summary: A language for scientific illustration
%define griversion 2.6.1
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
	if ! grep -i gri /usr/share/info/dir
	then
		/sbin/install-info --dir-file="/usr/share/info/dir" /usr/share/info/gri.info.gz
#		chmod a+r /usr/share/info/dir
	fi
fi

%postun
# remove instances of gri in /usr/share/info/dir
if [ "$1" = 0 ];
then
	if grep -i "gri" /usr/share/info/dir
	then
		/sbin/install-info --dir-file="/usr/share/info/dir" --remove /usr/share/info/gri.info.gz
#		grep -vi "gri" /usr/share/info/dir > /usr/info/share/dir.tmp
#		mv /usr/share/info/dir.tmp /usr/share/info/dir
#		chmod a+r /usr/share/info/dir
	fi
fi

%changelog
* Thu Apr 19 2001 Dan Kelley <dan.kelley@dal.ca>
- Rename this file as gri.spec, without the version number embedded 
  in the filename.  Upgrade to version number 2.6.1.  Change url to
  point to sourceforge site (but leave ftp as it is, for now anyway).

* Tue Jan 30 2001  Dan Kelley <dan.kelley@dal.ca>
- Changing to e.g. /usr/share/info instead of /usr/info.  Same for manpages.  I 
  know, I should be using the fancy macros that are defined in /usr/lib/rpm, but
  these seemed contradictory, with respect to where things are in my Redhat 7.0
  setup ... and I had a hard time figuring out how to use these macros anyway,
  so I just gave up and hard-wired them in, using the new directories as
  used in Redhat 7.0, as opposed to the (different) directories in all the other
  Redhat versions I've had.  Someday I'll switch to using macros, but
  it means changing both this spec-file and various Makefiles, and I need to
  be sure that changes to the Makefiles don't hurt the distributions for Debian
  linux, for solaris, etc.

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
