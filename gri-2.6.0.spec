Summary: a language for scientific illustration
Name: gri
Version: 2.6.0
Release: 1
Copyright: distributable
Group: Applications/Engineering
Source: ftp://ftp.phys.ocean.dal.ca/users/kelley/gri/gri-2.6.0.tgz
URL: http://www.phys.ocean.dal.ca/~kelley/gri/index.html
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
(cd doc ; make html-install HTML_DIR=../docinst/html)
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
/usr/share/gri
/usr/man/*/gri.*
/usr/man/*/gri_unpage.*
/usr/man/*/gri_merge.*
/usr/info/gri.info*
/usr/share/emacs/site-lisp/gri-mode.el
/usr/bin/gri_unpage
/usr/bin/gri_merge

%post
# add a gri entry to /usr/info/dir
if [ "$1" = 1 ]
then
	if ! grep -i gri /usr/info/dir
	then
#		/sbin/install-info --dir-file="/usr/info/dir" --entry="* GRI: (gri). Scientific graphics language." --info-file=/usr/info/gri.info
		/sbin/install-info --dir-file="/usr/info/dir" /usr/info/gri.info.gz
		chmod a+r /usr/info/dir
	fi
fi

%postun
# remove instances of gri in /usr/info/dir
if [ "$1" = 0 ];
then
	if grep -i "gri" /usr/info/dir
	then
		/sbin/install-info --dir-file="/usr/info/dir" --remove /usr/info/gri.info.gz
#		grep -vi "gri" /usr/info/dir > /usr/info/dir.tmp
#		mv /usr/info/dir.tmp /usr/info/dir
		chmod a+r /usr/info/dir
	fi
fi

%changelog
* Thu Jun 1 2000  Dan Kelley <dan.kelley@dal.ca>
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
