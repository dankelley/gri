AC_PREREQ(2.50)
AC_INIT(gri, 2.12.7)
AC_CONFIG_SRCDIR(src/gri.cc)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_MAKE_SET
PROGS="gri"

AC_SUBST(PROGS)

dnl     checks for programs.

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG_CPLUSPLUS

dnl	See if uses string::remove or string::erase
AC_DEFUN(CHECK_OLD_STRING,
AC_MSG_CHECKING(for old C++ subroutine called string::remove)
AC_CACHE_VAL(gri_cv_have_old_string, [
    AC_TRY_COMPILE([
#define _POSIX_SOURCE 1
#include <string>
], [
string s("hi");
s.remove(0,1);
], gri_cv_have_old_string=yes, gri_cv_have_old_string=no)])
    AC_MSG_RESULT($gri_cv_have_old_string)
    if test $gri_cv_have_old_string = yes ; then
        AC_DEFINE(HAVE_OLD_STRING)
    fi
    )dnl
dnl
CHECK_OLD_STRING

define(GCC_TWEAK1,
AC_MSG_CHECKING(for gcc compiler)
[AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#if defined(__GNUC__)
  yes
#endif
], GCC=yes, GCC=no)
if test $GCC = yes; then
  EXTRA_CFLAGS_TEMPLATE="$AM_CFLAGS -Wall"
  AM_CXXFLAGS="$AM_CXXFLAGS -Wall"
  AC_SUBST(AM_CXXFLAGS)
  AC_SUBST(EXTRA_CFLAGS_TEMPLATE)
fi
AC_MSG_RESULT($GCC)
])dnl
dnl
GCC_TWEAK1

define(GCC_TWEAK2,
AC_MSG_CHECKING(for C++ compiler with new headers as of August 2002)
[AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#if defined(__GNUC__)
#if __GNUC__ >= 3
  yes
#endif
#endif
], Cplusplusnew=yes, Cplusplusnew=no)
if test $Cplusplusnew = yes; then
  AM_CXXFLAGS="$AM_CXXFLAGS -DCPLUSPLUSNEW"
  AM_CXXFLAGS="$AM_CXXFLAGS -fno-strength-reduce"
  AC_SUBST(AM_CXXFLAGS)
fi
AC_MSG_RESULT($Cplusplusnew)
])dnl
dnl
GCC_TWEAK2

dnl Note: prior to 2.9.x, we used to have
dnl   EXTRA_CFLAGS="$EXTRA_CFLAGS -Wall -fno-implicit-templates"
dnl above.  But now, this seems not to be needed, at least
dnl on intel (linux), with g++-2.96 and g++-3.0

dnl On DEC computers, with cxx compiler, need something special
AC_DEFUN(DEK_IS_DECCXX,
AC_MSG_CHECKING(for DEC cxx compiler)
AC_CACHE_VAL(gri_cv_is_deccxx, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef __DECCXX
  yes
#endif
], gri_cv_is_deccxx=yes, gri_cv_is_deccxx=no)])
    AC_MSG_RESULT($gri_cv_is_deccxx)
    if test $gri_cv_is_deccxx = yes; then
        AM_CXXFLAGS="$AM_CXXFLAGS -D_ANSI_C_SOURCE"
        AC_SUBST(AM_CXXFLAGS)
    fi
)dnl
dnl
DEK_IS_DECCXX

dnl     See if apple (e.g. OS/X)
AC_DEFUN(DEK_IS_APPLE,
AC_MSG_CHECKING(for for apple computer)
AC_CACHE_VAL(gri_cv_is_apple, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#if defined(__APPLE__) && defined(__GNUC__)
  yes
#endif
], gri_cv_is_apple=yes, gri_cv_is_apple=no)])
    AC_MSG_RESULT($gri_cv_is_apple)
    if test $gri_cv_is_apple = yes; then
        AM_CXXFLAGS="$AM_CXXFLAGS -O0"
        AC_DEFINE(IS_APPLE)
    fi
)dnl
dnl
DEK_IS_APPLE


AC_DEFUN(IS_THIS_AN_ALPHA_SYSTEM,
AC_MSG_CHECKING(for DEC alpha computer)
AC_CACHE_VAL(gri_cv_is_dec_alpha, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef __alpha__
  yes
#endif
], gri_cv_is_dek_alpha=yes, gri_cv_is_dec_alpha=no)])
    AC_MSG_RESULT($gri_cv_is_dec_alpha)
    if test $gri_cv_is_dec_alpha = yes; then
        AC_DEFINE(IS_DEC_ALPHA)
    fi
)dnl
dnl
IS_THIS_AN_ALPHA_SYSTEM

AC_DEFUN(IS_THIS_A_BEOS_SYSTEM,
AC_MSG_CHECKING(for BeOS environment)
AC_CACHE_VAL(gri_cv_is_beos, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef __BEOS__
  yes
#endif
], gri_cv_is_beos=yes, gri_cv_is_beos=no)])
    AC_MSG_RESULT($gri_cv_is_beos)
    if test $gri_cv_is_beos = yes; then
        AC_DEFINE(OS_IS_BEOS)
    fi
)dnl
dnl
IS_THIS_A_BEOS_SYSTEM


dnl     See if HPUX system
AC_DEFUN(IS_THIS_AN_HPUX_SYSTEM,
AC_MSG_CHECKING(for HPUX environment)
AC_CACHE_VAL(gri_cv_is_hpux, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef hpux
  yes
#endif
], gri_cv_is_hpux=yes, gri_cv_is_hpux=no)])
    AC_MSG_RESULT($gri_cv_is_hpux)
    if test $gri_cv_is_hpux = yes; then
        AC_DEFINE(OS_IS_HPUX)
    fi
)dnl
dnl
IS_THIS_AN_HPUX_SYSTEM



dnl     See if SUN computer
AC_DEFUN(DEK_IS_SUN,
AC_MSG_CHECKING(for for Sun Microsystems computer)
AC_CACHE_VAL(gri_cv_is_sun, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef sun
  yes
#endif
], gri_cv_is_sun=yes, gri_cv_is_sun=no)])
    AC_MSG_RESULT($gri_cv_is_sun)
    if test $gri_cv_is_sun = yes; then
        AC_DEFINE(IS_SUN)
    fi
)dnl
dnl
DEK_IS_SUN

dnl     See if IBM AIX with IBM compiler.  Note
dnl	that an IBM running GNU compiler won't
dnl	fall into this category.
AC_DEFUN(DEK_IS_AIX_WITH_IBM,
AC_MSG_CHECKING(for for IBM AIX computer with IBM compiler)
AC_CACHE_VAL(gri_cv_is_aix_with_ibm, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef __IBMCPP__
  yes
#endif
], gri_cv_is_aix_with_ibm=yes, gri_cv_is_aix_with_ibm=no)])
    AC_MSG_RESULT($gri_cv_is_aix_with_ibm)
    if test $gri_cv_is_aix_with_ibm = yes; then
        AC_DEFINE(IS_AIX_WITH_IBM)
    fi
)dnl
dnl
DEK_IS_AIX_WITH_IBM


dnl     See if mingw32
AC_DEFUN(DEK_IS_MINGW32,
AC_MSG_CHECKING(for for mingw32 system)
AC_CACHE_VAL(gri_cv_is_mingw32, [
AC_BEFORE([$0], [AC_COMPILE_CHECK])AC_BEFORE([$0], [AC_TEST_PROGRAM])AC_BEFORE([$0], [AC_HEADER_EGREP])AC_BEFORE([$0], [AC_TEST_CPP])AC_EGREP_CPP(yes,
[#ifdef __MINGW32__
  yes
#endif
], gri_cv_is_mingw32=yes, gri_cv_is_mingw32=no)])
    AC_MSG_RESULT($gri_cv_is_mingw32)
    if test $gri_cv_is_mingw32 = yes; then
        AC_DEFINE(IS_MINGW32)
    fi
)dnl
dnl
DEK_IS_MINGW32

dnl Determine endian type.  NOTE: I have [[ and ]] in the test code.  I couldn't
dnl find anything in the docs that told me to do that, but it was the only
dnl I could find to make the code, created in "configure", to have the
dnl required brackets.  I tried the quoting macros as described in the docs
dnl but couldn't get it working.
AC_DEFUN(GRI_DETERMINE_ENDIAN,
	AC_MSG_CHECKING(to see whether this machine is big endian or little endian)
	AC_CACHE_VAL(gri_cv_endian_type, [
	AC_LANG_C
	AC_TRY_RUN([
		int main () {
			/* Figure out endian-ness, from Harbison & Steele */
			union {
				long l;
				char c[[sizeof (long)]];
			} u;
			u.l = 1;
			exit(u.c[[sizeof (long) - 1]] == (char)1);
		}], gri_cv_endian_type=little, gri_cv_endian_type=big, gri_cv_endian_type=unknown)])
	AC_LANG_CPLUSPLUS
	AC_MSG_RESULT($gri_cv_endian_type)
	if test $gri_cv_endian_type = big; then
		AC_DEFINE(GRI_IS_BIG_ENDIAN, 1)
	else
		AC_DEFINE(GRI_IS_BIG_ENDIAN, 0)
	fi
)dnl
dnl
GRI_DETERMINE_ENDIAN


AC_CHECK_HEADERS(unistd.h)


dnl     Check for netcdf library.  It might be
dnl	in one of several places, so check
dnl	several locations, reporting location
dnl	if found.
AC_MSG_CHECKING(for netcdf library)
define(CHECK_FOR_NETCDF, [
    if test -d /opt/netcdf/include ; then
	#
	# Solaris often has it here
	AM_CXXFLAGS="$AM_CXXFLAGS -I/opt/netcdf/include"
        EXTRA_CFLAGS_TEMPLATE="$EXTRA_CFLAGS_TEMPLATE -I/opt/netcdf/include"
        DEFS="$DEFS -I/opt/netcdf/include"
dnl BUG: FIX THIS BY DUPLICATING BELOW; ALSO REMOVE EXTRA_CFLAGS
        LIBS="$LIBS -L/opt/netcdf/lib -lnetcdf"
	if test $gri_cv_is_sun = yes ; then
	    LIBS="$LIBS -lnsl"
	fi
	AC_DEFINE(HAVE_LIBNETCDF)
        echo "yes (in /opt/netcdf/include/netcdf.h and /opt/netcdf/lib/libnetcdf.a)"
    else
	if test -f /usr/local/include/netcdf.h ; then
	    AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/local/include"
            EXTRA_CFLAGS_TEMPLATE="$EXTRA_CFLAGS_TEMPLATE -I/opt/netcdf/include"
	    LIBS="$LIBS -L/usr/local/lib -lnetcdf"
            if test $gri_cv_is_sun = yes ; then
		LIBS="$LIBS -lnsl"
	    fi
	    AC_DEFINE(HAVE_LIBNETCDF)
            echo "yes (in /usr/local/include/netcdf.h and /usr/local/lib/libnetcdf.a)"
	else
	    #
	    # Linux/debian often has it here.
	    if test -f /usr/include/netcdf.h ; then
                AM_CXXFLAGS="$AM_CXXFLAGS -I/usr/include/netcdf"
                EXTRA_CFLAGS_TEMPLATE="$EXTRA_CFLAGS_TEMPLATE -I/opt/netcdf/include"
                LIBS="$LIBS -L/usr/lib/netcdf -lnetcdf"
                if test $gri_cv_is_sun = yes ; then
                    LIBS="$LIBS -lnsl"
                fi
                AC_DEFINE(HAVE_LIBNETCDF)
                echo "yes (in /usr/include/netcdf.h and /usr/lib/libnetcdf.a)"
            else
		if test -f /boot/home/lib/netcdf ; then
		    AM_CXXFLAGS="$AM_CXXFLAGS -I/boot/home/include/netcdf"
                    EXTRA_CFLAGS_TEMPLATE="$EXTRA_CFLAGS_TEMPLATE -I/opt/netcdf/include"
		    LIBS="$LIBS -L/boot/home/lib/netcdf -lnetcdf"
                    if test $gri_cv_is_sun = yes ; then
			LIBS="$LIBS -lnsl"
		    fi
                    echo "yes (in /boot/home/netcdf)"
		    AC_DEFINE(HAVE_LIBNETCDF)
		else
	            echo "no"
		fi
            fi
        fi
    fi
])dnl
dnl
CHECK_FOR_NETCDF

dnl use this??
dnl AC_SUBST(EXTRA_CXXFLAGS)
#AC_SUBST(EXTRA_OBJS)
dnl AC_SUBST(EXTRA_LIBS)


dnl	Check for Imgemagick's convert, needed to make PNG files in HTML manual
AC_CHECK_PROG(HAVE_CONVERT, convert, "yes", "no")
if test $HAVE_CONVERT = "no" ; then
	echo " **WARNING:  Cannot make proper HTML manual without Imagemagick."
	echo " **WARNING:  It is needed to make PNG images from PostScript files."
	echo " **WARNING:  Expect broken image links in the manual!"
fi

dnl	AC_MSG_CHECKING(for ImageMagick's convert)
dnl	define(CHECK_FOR_CONVERT, [
dnl		if test -f /usr/bin/convert ; then
dnl			HAVE_CONVERT="yes"
dnl			echo "yes"
dnl		else
dnl			HAVE_CONVERT="no"
dnl			echo "no.  Cannot make HTML manual without it"
dnl		fi
dnl		AC_SUBST(HAVE_CONVERT)dnl
dnl	])dnl
dnl	dnl
dnl	CHECK_FOR_CONVERT

dnl     checks for UNIX variants that set `DEFS'

dnl     checks for header files
dnl     stdarg.h, stdlib.h
AC_HEADER_STDC

dnl	Check whether system has libpopt online.  For
dnl	now, this check value is not used; later on 
dnl	I should skip the compilation of my local
dnl	popt if there is a version online.
AC_CHECK_LIB(popt, poptReadConfigFile, AM_CONDITIONAL(OS_HAS_POPT, test 1=1), AM_CONDITIONAL(OS_HAS_POPT, test 0=1))

dnl     checks for typedefs
AC_TYPE_PID_T

dnl     Check for POSIX times()
AC_DEFUN(CHECK_FOR_POSIX_TIMES,
AC_MSG_CHECKING(for POSIX times() subroutine)
AC_CACHE_VAL(gri_cv_have_posix_times, [
    AC_TRY_COMPILE([
#define _POSIX_SOURCE 1
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/times.h>
], [
struct tms buffer; 
#ifdef sun
broken
#endif
clock_t t = times(&buffer);
], gri_cv_have_posix_times=yes, gri_cv_have_posix_times=no)])
    AC_MSG_RESULT($gri_cv_have_posix_times)
    if test $gri_cv_have_posix_times = yes ; then
        AC_DEFINE(HAVE_POSIX_TIMES)
    fi
    )dnl
dnl
CHECK_FOR_POSIX_TIMES


dnl     checks for functions
dnl AC_CHECK_FUNCS(isnan isinf acosh getcwd getenv popen drand48 mkstemp tmpnam tempname gethostname access lstat strerror)
AC_CHECK_FUNCS(isnan isinf acosh getcwd popen mkstemp tmpnam tempnam gethostname access lstat stat strerror)
dnl The next lines are because configure fails in c++, to find these things.
dnl I first noticed this on 2002-Dec-15.  A discussion thread on the topic,
dnl valid at that date, is 
dnl     http://mail.gnu.org/pipermail/bug-autoconf/2001-September/001036.html
dnl and I patterned my solution loosely on advice I found thre.
AC_CHECK_DECL(getenv, [AC_DEFINE(HAVE_GETENV)])
AC_CHECK_DECL(drand48, [AC_DEFINE(HAVE_DRAND48)])

dnl     checks for structure members

dnl     checks for compiler characteristics

dnl     checks for operating system services

dnl     other checks for UNIX variants

dnl See if the user did   "configure --enable-linux_redhat"
AC_ARG_ENABLE(linux_redhat,
[  --enable-linux_redhat    Build for Linux/redhat OS],
[case "${enableval}" in
  yes) linux_redhat=true ;;
  no)  linux_redhat=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-linux_redhat) ;;
esac],[debug=false])
AM_CONDITIONAL(OS_IS_LINUX_REDHAT, test x$linux_redhat = xtrue)
dnl use as e.g.
dnl if OS_IS_LINUX_REDHAT
dnl NAME = Dan
dnl else
dnl NAME = nad
dnl endif
dnl myname = $(NAME)
dnl 
dnl dan:
dnl 	@echo "myname is $(myname)"

dnl See if the user did   "configure --enable-linux_debian"
AC_ARG_ENABLE(linux_debian,
[  --enable-linux_debian    Build for Linux/debian OS],
[case "${enableval}" in
  yes) linux_debian=true ;;
  no)  linux_debian=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-linux_debian) ;;
esac],[debug=false])
AM_CONDITIONAL(OS_IS_LINUX_DEBIAN, test x$linux_debian = xtrue)

dnl See if the user did   "configure --enable-solaris"
AC_ARG_ENABLE(solaris,
[  --enable-solaris    Build for Solaris OS],
[case "${enableval}" in
  yes) solaris=true ;;
  no)  solaris=false ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-solaris) ;;
esac],[debug=false])
AM_CONDITIONAL(OS_IS_SOLARIS, test x$solaris = xtrue)

AC_OUTPUT([Makefile debian/Makefile doc/Makefile doc/examples/Makefile doc/tst_suite/Makefile doc/resources/Makefile doc/screenshots/Makefile src/Makefile src/popt/Makefile])
