<!-- -*- html -*- -->

<!-- HOW TO INSTALL ON SOURCEFORGE:

scp index.php dankelley@gri.sourceforge.net:/home/groups/g/gr/gri/htdocs
scp index.php psg@gri.sourceforge.net:/home/groups/g/gr/gri/htdocs

-->


<html>
  <head>
    <title>Gri</title>
    <meta name="Copyright" content="2002 Dan Kelley, Dalhousie University, Halifax, Nova Scotia, Canada">
    <meta name="Author" content="Dan Kelley, Dan.Kelley@Dal.Ca">
    <meta name="keywords" content="Gri, graphics, Scientific Computing">
    <!--    <LINK rel="stylesheet" href="./gri.css" type="text/css"> -->

    <style type="text/css">
    <!--
    BODY { background-color: #FFFFFF }

    A { text-decoration: none; color #6666DD; }
    A:visited { text-decoration: none; color: #6666DD; }
    A:link { text-decoration: none; color: #6666DD; }
    A:active { text-decoration: none; color: #6666DD; }
    A:hover { text-decoration: underline; color: #6666DD; }
    A:mail { text-decoration: none; color: #6666DD; font-weight: bold }
    OL,UL,P,BODY,TD,TR,TH,FORM,SPAN { font-family: arial,helvetica,sans-serif;color: #333333 }
    H1,H2,H3,H4,H5,H6 { font-family: arial,helvetica,sans-serif }
    PRE,TT { font-family: courier,sans-serif }

    SPAN.center { text-align: center }
    SPAN.boxspace { font-size: 2pt; }

    A.maintitlebar { color: #FFFFFF; }
    A.bottombar { color: #FFFFFF; text-decoration: none;  }
    A.sortbutton { color: #FFFFFF; text-decoration: none; }
    A.menus { color: #6666DD; text-decoration: none; }
    A.menus:hover { text-decoration: underline; }
    A.tabs { color: #000000; text-decoration: none; }
    A.date { color: #ff0000; text-decoration: none; }
    SPAN.alignright { text-align: right }
    SPAN.maintitlebar { font-size: 10pt; color: #FFFFFF; font-weight: bold  }
    SPAN.bottombar { font-size: 10pt; color: #FFFFFF; font-weight: bold }
    SPAN.titlebar { text-align: center; font-size: 10pt; color: #FFFFFF; font-weight: bold }
    TD.featurebox { font-size: small; }
    TD.newsbox { font-size: 8pt; }
    TD.menus { color: #000000; }
    -->
    </style>
  </head>

  <body bgcolor=#FFFFFF topmargin="0" bottommargin="0" leftmargin="0" rightmargin="0" marginheight="0" marginwidth="0">

    <TABLE width="100%" border=0 cellspacing=0 cellpadding=0 bgcolor="#737b9c">
      <TR>
        <TD align="left" width="100%">
          <SPAN class=maintitlebar>
	  <A class="maintitlebar" href="http://sourceforge.net/">
	    <IMG src="http://sourceforge.net/sflogo.php?group_id=5511&type=1" width="88" height="31" border="0" alt="SourceForge Logo">
	  </A>
	  </span>
        </td>
      </tr>
    </table>

    <table bgcolor="#EEEEF8" width="100%" border=0 cellspacing=0 cellpadding=0>
      <TR bgcolor="#EEEEF8">
        <td align="center">
          <b><font size="+2">
            Gri: a language for scientific illustration</font>
          </b>
        </td>
      </tr>
    </table>
    
    <!-- LEFT COLUMN -->
    <table width="100%" cellspacing=0 cellpadding="2" border=0>
      <tr>
        <td valign="top" width="165" bgcolor="#EEEEF8">  <!-- light color -->
          <table width="100%" cellspacing=0 border=0 cellpadding=2 bgcolor="#EEEEF8">
	    <!-- DOWNLOAD STABLE -->
            <tr>
              <TD align="center" bgcolor="#737b9c">
                <span class="maintitlebar"><font color="#ffffff">Stable Release 2.10.1</font></span>
                </td> </tr>
                <tr>
                  <td class="menus" align="right">
		    <b>Source</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.10.1.tgz">Tarball (unbuilt docs)</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.10.1-arch-indep.tar.gz">Tarball (built docs)</a><br>
		    <b>RedHat Linux</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.10.1-1.src.rpm">Source RPM</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.10.1-1.i386.rpm">Intel RPM</a><br>
		    <b>Debian GNU/Linux</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri_2.8.6-0potato1_i386.deb">Old package for Potato</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri_2.10.1-1_i386.deb">Package for Woody</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.10.1static_2.10.1-1_i386.deb">Static, no docs</a><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/science/gri.html">gri at Debian</A><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/doc/gri-html-doc.html">HTML manual</A><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/doc/gri-ps-doc.html">PS manual</A><br>
		    <b>Other Systems</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.8.7-SunOS5.tar.gz">OLD: Solaris OS5</a><br>
		    <A class="menus" HREF="http://gnuwin32.sourceforge.net/packages/gri.htm">Windows</a><br>
		    <A class="menus" HREF="http://fink.sourceforge.net/pdb/package.php/gri">Macintosh OS-X</a><br>
                  </td>
                </tr>
            </tr>

	    <!-- DOWNLOAD UNSTABLE -->
            <tr> 
              <TD align="center" bgcolor="#737b9c">
                <span class="maintitlebar"><font color="#ffffff">Unstable Release</font></span>
              </td>
            </tr>
            <td class="menus" align="right">
            </td>
            <tr>
              <td class="menus" align="right">
<!--
                <b>None available yet</b>
 -->
		<b>General tarball</b><br>
                  <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs/gri_2.9.0.orig.tar.gz">CVS tar 2002-04-09</A><br>
                <b>Linux Pre-packaged</b><br>
                  <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs.html">CVS Debian packages</A><br>
              </td>
            </tr>

	    <!-- GENERAL DOWNLOAD AREA -->
            <tr> 
              <TD align="center" bgcolor="#737b9c">
                <span class="maintitlebar"><font color="#ffffff">Get Any Release</font></span>
              </td>
            </tr>
            <td class="menus" align="right">
            </td>
            <tr>
              <td class="menus" align="right">
                <a class="menus" href="http://sourceforge.net/project/showfiles.php?group_id=5511">General download</a><br>
              </td>
            </tr>
          </table>
      </td>

      <!-- Main Window -->
      <TD VALIGN="top" ALIGN="left" >
      <table align="center" width="95%"><tr><td>
          
Gri is a language for scientific graphics programming.  The word
"language" is important: Gri is command-driven, not point/click.  Some
users consider Gri similar to LaTeX, since both provide extensive
power as a reward for tolerating a learning curve.

<p>
Gri can make 
<a href="./gridoc/html/X-Y.html">
x-y
</a> graphs,
<a href="./gridoc/html/ContourPlots.html">
contour
</a> graphs, and 
<a href="./gridoc/html/Images.html">
image
</a>
graphs.  The output is in PostScript.  Fine control is provided over all aspects of drawing,
e.g. line widths, colors, and fonts.  A TeX-like syntax provides the
ability to use common Mathematical symbols.

<p>Folks who write thousand-line Gri scripts usually start with something
as simple as the following ...
<pre>
<font color="#82140F">
   # Draw a linegraph
   open file.dat
   read columns x y
   draw curve
</font>
</pre>
... which produces a PostScript graph with axes autoscaled to fit the
columnar data in the <tt><font color="#82140F">file.dat</font></tt>
file.

<p>

<hr>
<!-- See Ullman (2001) p33 for hints on how this PHP done -->
<form action="search_concept.php">
Search gri manual for (may be a regular expression):<p>
<input type=TEXT name=search_for size=30>
<input type=SUBMIT name="Submit" value="Submit"> 
</form>

        <hr noshade="noshade">
        <center>
          <small>
            Copyright &copy; 2002 by Dan Kelley and Peter Galbraith<br>
            This material may be distributed only subject to the terms and conditions 
            set forth in the GNU Publication License
          </small>
        </center>
        <!-- close the main window -->      
            </TD>
          </tr>
        </table>
</td>

<!-- right menus -->
<TD WIDTH="155" VALIGN="top" ALIGN="right" bgcolor="#EEEEF8">
  
  <!-- This Version -->
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr  bgcolor="#737b9c"> 
      <td align="center"> 
        <span class="maintitlebar"><font color="#ffffff">Present Version</font></span>
      </td>
    </tr>
    <tr> 
      <td  class="menubox">
	Version
	<a href="http://sourceforge.net/project/showfiles.php?group_id=5511&release_id=92480">2.10.1</a>
	released
	<? 
	# REF: mktime(hour,minute,second,month,day,year)
	#$diff = time() - mktime(7, 24, 0, 2, 14, 2002);
	# Vsn 2.8.7:  2002-04-03 18:10 Halifax time, 14:10 SF time
	#$diff = time() - mktime(14, 10, 0, 4, 3, 2002);
	# Vsn 2.10.0: 2002-05-20 09:15 Halifax time, 5:15 SF time
	#$diff = time() - mktime(5, 15, 0, 5, 20, 2002);
	# Vsn 2.10.1: 2002-06-1 15:36 Halifax time, 11:36 SF time
	$diff = time() - mktime(11, 36, 0, 6, 1, 2002);
	$days = floor($diff / 24 / 60 / 60);
	$weeks = floor($days / 7);
	$months = floor($days / 7 / 4);
	$hours = floor(($diff - $days * 24) / 60 / 60);
	$minutes = floor(floor($diff - $days * 24 * 60 * 60 - $hours * 60 * 60) / 60);
	$seconds = floor($diff - $days * 24 * 60 * 60 - $hours * 60 * 60 - $minutes * 60);
	# Below I break up into intervals that make sense, e.g.
	# only switching to months if it's been 3 or more.
	if ($months > 2) {
	    echo "$months months ago";
        } else if ($weeks > 4) {
	    echo "$weeks weeks ago";
	} else if ($days > 1) {
	    echo "$days days ago";
	} else if ($hours > 1) {
	    echo "$hours hours ago";
	} else if ($minutes > 1) {
	    echo "$minutes minutes ago";
	} else {
	    echo floor($seconds), " seconds ago";
        }
	?>
      </td>
    </tr>
  </table>

   <!-- Next Version -->
   <table width="100%" border="0" cellspacing="0" cellpadding="0">
     <tr  bgcolor="#737b9c"> 
       <td align="center"> 
	 <span class="maintitlebar"><font color="#ffffff">Next Version</font></span>
       </td>
     </tr>
     <tr>
       <td align="left">
       <A class="menus" HREF="http://gri.sourceforge.net/Development_Version.html">Changes</a><br>
       <A class="menus" HREF="http://gri.sourceforge.net/Plans.html">Plans</a><br>
<!--       <A class="menus" HREF="http://gri.sourceforge.net/pre1">Get Preview</A><br> -->
       </td>
     </tr>
   </table>

  <!-- Documentation -->
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr bgcolor="#737b9c"> 
      <td align="center"> 
            <span class="maintitlebar"><font color="#ffffff">Documentation</font></span>
      </td>
    </tr>
    <tr>
      <td class="menus" align="left">
        <A class="menus" HREF="http://gri.sourceforge.net/gridoc/html/Version_2_10.html#Version2.10">History of 2.10.x</a><br>
        <A class="menus" HREF="./gridoc/html/index.html">HTML manual</A><br>
        <A class="menus" HREF="./gri.pdf">PDF manual</A><br>
        <A class="menus" HREF="./gridoc/html/FAQ.html">FAQ</A><br>
        <A class="menus" HREF="./gri-cookbook/index.html">Cookbook</A><br>
        <A class="menus" HREF="./refcard.pdf">Basic refcard</A><br>
        <A class="menus" HREF="./cmdrefcard.pdf">Command refcard</A><br>
        <A class="menus" HREF="http://www2.linuxjournal.com/lj-issues/issue75/3743.html"><i>Linux Journal</i> article</A><br>
      </td>
    </tr>
  </table>


  <!-- Contacts -->
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr bgcolor="#737b9c"> 
      <td align="center"> 
            <span class="maintitlebar"><font color="#ffffff">Contacts</font></span>
      </td>
    </tr>
    <tr>
      <td class="menus" align="left">
        <A class="menus" href="http://www.sourceforge.net/news/?group_id=5511">News forum</a></br>
	<A class="menus" HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16974">Open forum</A><br>
	<A class="menus" HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16976">Developer forum</A><br>
	<A class="menus" HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16975">Help forum</A><br>
	<A class="menus" HREF="http://sourceforge.net/tracker/?atid=105511&group_id=5511&func=browse">Bug reports</A><br>
	<A class="menus" HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">Feature requests</A><br>
	<!-- <A class="menus" HREF="http://www.sourceforge.net/survey/?group_id=5511">Public surveys</A><br> -->
<!--	<A class="menus" HREF="http://www.sourceforge.net/mail/?group_id=5511">Mailing lists</A><br> --> 
	<A class="menus" HREF="mailto:dankelley@users.sourceforge.net">Email the author</A><br>
      </td>
    </tr>
  </table>
      

  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <TR>
      <TD align="center" bgcolor="#737b9c">
        <span class="maintitlebar"><font color="#ffffff">Developer's Corner</font></span>
      </TD>
    </TR>
    <TR>
      <td class="menus" align="left">
        <A class="menus" HREF="http://www.sourceforge.net/project/?group_id=5511">Developers' Page</A><br>
        <A CLASS="menus" HREF="http://cvs.sourceforge.net/cvstarballs/gri-cvsroot.tar.gz">Nightly CVS tarball</a><br>
        <A class="menus" HREF="http://www.sourceforge.net/cvs/?group_id=5511">Get CVS source</a><br>
        <A class="menus" HREF="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gri/gri/">View CVS history</a><br>
<!--	<a class="menus" href="http://sourceforge.net/project/showfiles.php?group_id=5511">File download area</a><br> -->
        <A class="menus" HREF="http://www.sourceforge.net/patch/?group_id=5511">Submit a patch</A><br>
        <A class="menus" HREF="http://sourceforge.net/pm/task.php?group_project_id=8706&group_id=5511&func=browse">View to-do list</A><br>
<!-- <A class="menus" HREF="./checklist_for_release">Packaging checklist</A><br> -->
        <br>

      </TD>
    </TR>
  </table>


  </td>
  </tr>
  </table>

<!--
    <table width="100%" border=0 cellspacing=0 cellpadding=0 bgcolor="737b9c">
      <tr>
        <td  align="center"><SPAN class=bottombar><font color="#fffff">Comments to </font> <A class=bottombar
              href="mailto:dankelley@users.sourceforge.net">webmaster</A></span></td>
        <td align="right" width="200"><SPAN class=bottombar><IMG src="blank.gif" height="1" width="145" border=0 alt=""></span></td>
      </tr>

    </table>
-->
  </body>
</html>
