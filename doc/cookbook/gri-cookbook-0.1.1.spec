#
# RPM specification file for gri
# 
# Patterned on p127 et seq. in Bailey (1997)
#
Summary: cookbook of examples for Gri graphics language
Name: gri-cookbook
Version: 0.1.1
Release: 1
Copyright: Dan E. Kelley
Group: Applications/Graphics
Source: ftp://ftp.phys.ocean.dal.ca/users/kelley/gri/gri-cookbook-0.1.1.tgz
URL: http://www.phys.ocean.dal.ca/~kelley/gri/gri1.html
Vendor: Tantramar Software
Packager: Dan E. Kelley <Dan.Kelley@Dal.CA>
Icon: grilogo.gif

# Not really sure what to put for the following, so comment out.
#Provides: ?
#Distribution: ?



%description

Cookbook of sample Gri programs.

Gri is a language for scientific graphics programming.  It is a
command-driven application, as opposed to a click/point application.
It is analogous to latex, and shares the property that extensive power
is the reward for tolerating a modest learning curve.  Gri output is
in industry-standard PostScript, suitable for incorporation in
documents prepared by various text processors.

%prep
%setup

%build
make all

%install
make install_linux_redhat


%post

%files
/opt/gri/doc/cookbook/cookbook.html
/opt/gri/doc/cookbook/discussion.html
/opt/gri/doc/cookbook/index.html
/opt/gri/doc/cookbook/recipes.html
/opt/gri/doc/cookbook/submitting.html

/opt/gri/doc/cookbook/1/example-small.gif
/opt/gri/doc/cookbook/1/example.gif
/opt/gri/doc/cookbook/1/example.html

/opt/gri/doc/cookbook/2/example-small.gif
/opt/gri/doc/cookbook/2/example.gif
/opt/gri/doc/cookbook/2/example.html

/opt/gri/doc/cookbook/3/example-small.gif
/opt/gri/doc/cookbook/3/example.gif
/opt/gri/doc/cookbook/3/example.html

/opt/gri/doc/cookbook/4/example-small.gif
/opt/gri/doc/cookbook/4/example.gif
/opt/gri/doc/cookbook/4/example.html

/opt/gri/doc/cookbook/5/example-small.gif
/opt/gri/doc/cookbook/5/example.gif
/opt/gri/doc/cookbook/5/example.html

/opt/gri/doc/cookbook/6/example-small.gif
/opt/gri/doc/cookbook/6/example.gif
/opt/gri/doc/cookbook/6/example.html

/opt/gri/doc/cookbook/7/example-small.gif
/opt/gri/doc/cookbook/7/example.gif
/opt/gri/doc/cookbook/7/example.html

/opt/gri/doc/cookbook/8/example-small.gif
/opt/gri/doc/cookbook/8/example.gif
/opt/gri/doc/cookbook/8/example.html

/opt/gri/doc/cookbook/9/example-small.gif
/opt/gri/doc/cookbook/9/example.gif
/opt/gri/doc/cookbook/9/example.html

/opt/gri/doc/cookbook/10/example-small.gif
/opt/gri/doc/cookbook/10/example.gif
/opt/gri/doc/cookbook/10/example.html

/opt/gri/doc/cookbook/11/example-small.gif
/opt/gri/doc/cookbook/11/example.gif
/opt/gri/doc/cookbook/11/example.html

/opt/gri/doc/cookbook/12/example-small.gif
/opt/gri/doc/cookbook/12/example.gif
/opt/gri/doc/cookbook/12/example.html
/opt/gri/doc/cookbook/12/seawifs.palette
/opt/gri/doc/cookbook/12/example-vertical.gri


%postun
# Clean up, for uninstallation
# ref: Bailey 13.3.2.4; 20.4.3.2
#
# Note: as in Bailey (p182), this will be called with argument of 
# zero when the last version of Gri is uninstalled.  So that's the time
# to clean up the info-dir database, and not before.

# Clean up line in the info directory
if [ "$1" = 0 ];
then
	rm -rf /opt/gri/doc/cookbook
fi
