<!-- -*- html -*- -->

<!-- HOW TO INSTALL ON SOURCEFORGE:

scp index.php dankelley@gri.sourceforge.net:/home/groups/g/gr/gri/htdocs
scp index.php psg@gri.sourceforge.net:/home/groups/g/gr/gri/htdocs

-->


<html>
  <head>
    <title>[Gri] Gri Site</title>
    <meta name="Copyright" content="2001 Dan Kelley, Dalhousie University, Halifax, Nova Scotia, Canada">
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

    A.maintitlebar { color: #FFFFFF;  }
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
        <td align="left" width="50%">
          <SPAN class=maintitlebar><A class="maintitlebar" href="http://sourceforge.net/"><IMG SRC="./sflogo.png" NAME="Graphic3" ALIGN=MIDDLE BORDER=0></A></span>
        </td>
        <td align="right" width="50%"><SPAN class=maintitlebar><IMG src="./images/blank.gif" height="1" width="145" border=0 alt=""></span></td>
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
                <IMG src="./images/blank.gif" height="1" width="1" border=0 alt=""><BR>
                <span class="maintitlebar"><font color="#ffffff">Download Stable Release</font></span>
                </td> </tr>
                <tr>
                  <td class="menus" align="right">
		    <b>Source tar ball</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.8.0.tgz">gri-2.8.0.tgz</a><br>
		    <b>RedHat Linux</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.8.0-1.src.rpm">gri-2.8.0-1.src.rpm</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.8.0-1.i386.rpm">gri-2.8.0-1.i386.rpm</a><br>
		    <b>Debian GNU/Linux</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri_2.8.0-0potato1_i386.deb">2.8.0 for Potato</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri_2.8.0-0woody1_i386.deb">2.8.0 for Woody</a><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/science/gri.html">gri at Debian</A><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/doc/gri-html-doc.html">HTML manual at Debian</A><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/doc/gri-ps-doc.html">PS manual at Debian</A><br>
		    <b>Solaris Unix</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.6.0-SunOS5.tar.gz">gri-2.6.0-SunOS6.tar.gz</a><br>
		    <b>Windows</b><br>
		    <A class="menus" HREF="http://gnuwin32.sourceforge.net/packages/gri.htm">Win95/98/2000/NT port</a><br>
                  </td>
                </tr>
            </tr>

	    <!-- DOWNLOAD UNSTABLE -->
            <tr> 
              <TD align="center" bgcolor="#737b9c">
                <IMG src="./images/blank.gif" height="1" width="1" border=0 alt=""><BR>
                <span class="maintitlebar"><font color="#ffffff">Download Unstable Release</font></span>
              </td>
            </tr>
            <td class="menus" align="right">
            </td>
            <tr>
              <td class="menus" align="right">
                <b>None available yet</b>
<!--
		<b>General tarball</b><br>
                  <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs/gri_2.7.0.orig.tar.gz">Recent CVS sources</A><br>
                  <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs/gri_2.8.0.orig.tar.gz">2.8.0 pre-release #1</A><br>
                <b>Linux Pre-packaged</b><br>
                  <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs.html">CVS Debian packages</A><br>
                  <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs.html">2.8.0 pre-1 for Debian</A><br>
 -->
              </td>
            </tr>

	    <!-- GENERAL DOWNLOAD AREA -->
            <tr> 
              <TD align="center" bgcolor="#737b9c">
                <IMG src="./images/blank.gif" height="1" width="1" border=0 alt=""><BR>
                <span class="maintitlebar"><font color="#ffffff">Download Any Release</font></span>
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
          
Gri is an open-source language for scientific graphics programming.
It is command-driven, as opposed to point/click, and its output is a
PostScript file.  Some users consider Gri similar to LaTeX, since both
provide extensive power as a reward for tolerating a learning curve.

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
graphs.  Fine control is provided over all aspects of drawing,
e.g. line widths, colors, and fonts.  A TeX-like syntax provides the
ability to use common Mathematical symbols.

<p>Folks who write 1000-line Gri scripts usually start with something
as simple as the following.
<pre>
<font color="#82140F">
   # Draw a linegraph
   open file.dat
   read columns x y
   draw curve
</font>
</pre>

To see the recent changes, go <A class="menus"
HREF="http://gri.sourceforge.net/gridoc/html/Version_2_8.html#Version2.8">here</a>.<br>

<p>
<!-- See Ullman (2001) p33 for hints on how this PHP done -->
<form action="search_concept.php">
Search gri manual for (may be a regular expression):<p>
<input type=TEXT name=search_for size=30>
<input type=SUBMIT name="Submit" value="Submit"> 
</form>

        <hr noshade="noshade">
        <center>
          <small>
            Copyright &copy; 2001 by Dan Kelley<br>
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
  
  <!-- Status -->
  <table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr  bgcolor="#737b9c"> 
      <td align="center"> 
        <span class="maintitlebar"><font color="#ffffff">Status</font></span>
      </td>
    </tr>
    <tr> 
      <td  class="menubox">
        <!-- Vsn 2.8.0: 2001-jul-27</br> -->
	Stable version 
	<a href="http://sourceforge.net/project/showfiles.php?group_id=5511&release_id=45649">2.8.0</a>
	released
	<? 
	# mktime(hour,minute,second,month,day,year)
	$diff = time() - mktime(12, 29, 0, 7, 27, 2001);
	$days = floor($diff / 24 / 60 / 60);
	$weeks = floor($days / 7);
	$months = floor($days / 7 / 4);
	$hours = floor(($diff - $days * 24) / 60 / 60);
	$minutes = floor($diff - $days * 24 * 60 * 60 - $hours * 60 * 60) / 60;
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

   <!-- News -->
   <table width="100%" border="0" cellspacing="0" cellpadding="0">
     <tr  bgcolor="#737b9c"> 
       <td align="center"> 
	 <span class="maintitlebar"><font color="#ffffff">News</font></span>
       </td>
     </tr>
     <tr> 
       <td  class="menus" align="left">
	 <a class="menus" href="http://www.sourceforge.net/news/?group_id=5511">News forum</a></b>
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
        <A class="menus" HREF="./gridoc/html/index.html">HTML manual</A><br>
        <A class="menus" HREF="./gri.pdf">PDF manual</A><br>
        <A class="menus" HREF="./gridoc/html/FAQ.html">FAQ</A><br>
        <A class="menus" HREF="./gri-cookbook/index.html">Cookbook of examples</A><br>
        <A class="menus" HREF="./refcard.ps">Reference card</A><br>
        <A class="menus" HREF="./cmdrefcard.ps">Command refcard</A><br>
        <A  class="menus" HREF="http://www2.linuxjournal.com/lj-issues/issue75/3743.html">Article in <i>Linux Journal</i></A><br>
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
	<A class="menus" HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16974">Discussion forum</A><br>
	<A class="menus" HREF="http://www.sourceforge.net/forum/forum.php?forum_id=16975">Help forum</A><br>
	<A class="menus" HREF="http://sourceforge.net/tracker/?atid=105511&group_id=5511&func=browse">Bug reports</A><br>
	<A class="menus" HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">Feature requests</A><br>
	<!-- <A class="menus" HREF="http://www.sourceforge.net/survey/?group_id=5511">Public surveys</A><br> -->
	<A class="menus" HREF="http://www.sourceforge.net/mail/?group_id=5511">Mailing lists</A><br> 
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
        <A class="menus" HREF="http://www.sourceforge.net/project/?group_id=5511">SourceForge Gri page</A><br>
        <A class="menus" HREF="http://www.sourceforge.net/cvs/?group_id=5511">Get CVS source</a><br>
        <A class="menus" HREF="http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/gri">View CVS history</a><br>
<!--	<a class="menus" href="http://sourceforge.net/project/showfiles.php?group_id=5511">File download area</a><br> -->
        <A class="menus" HREF="http://www.sourceforge.net/patch/?group_id=5511">Submit a patch</A><br>
        <A class="menus" HREF="http://sourceforge.net/pm/task.php?group_project_id=8706&group_id=5511&func=browse">View to-do list</A><br>
<!--        <a class="menus" href="./gri-2.8.0-pre1.tgz"><font color="red">2.8.0 pre-release #1</font></a>  -->
<!-- <A class="menus" HREF="http://gri.sourceforge.net/gridoc/html/Version_2_6.html#Version2.6">Version 2.6.1 preview</A><br> -->
<!-- <A class="menus" HREF="./checklist_for_release">Packaging checklist</A><br> -->
      </TD>
    </TR>
  </table>




  </td>
  </tr>
  </table>

    <table width="100%" border=0 cellspacing=0 cellpadding=0 bgcolor="737b9c">
      <tr>
        <td  align="center"><SPAN class=bottombar><font color="#fffff">Comments to </font> <A class=bottombar
              href="mailto:dankelley@users.sourceforge.net">webmaster</A></span></td>
        <td align="right" width="200"><SPAN class=bottombar><IMG src="blank.gif" height="1" width="145" border=0 alt=""></span></td>
      </tr>





    </table>
  </body>
</html>

