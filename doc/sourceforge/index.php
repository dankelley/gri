      <!-- -*- html -*- -->

<!-- HOW TO INSTALL ON SOURCEFORGE:

  scp index.php dankelley@gri.sourceforge.net:/home/groups/g/gr/gri/htdocs
  scp index.php psg@gri.sourceforge.net:/home/groups/g/gr/gri/htdocs

-->


<!-- colors (copied from SF colors):
  dark, under headings headings:        #737b9c
  light, table background               #EEEEF8
-->

<html>
  <head>
    <title>[Gri] Gri Site</title>
    <meta name="Copyright" content="2001 Dan Kelley, Dalhousie University, Halifax, Nova Scotia, Canada">
    <meta name="Author" content="Dan Kelley, Dan.Kelley@Dal.Ca">
    <meta name="keywords" content="Gri, graphics, Scientific Computing">
    <LINK rel="stylesheet" href="./gri.css" type="text/css">
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
          <font size="+4">Gri: a language for scientific illustration</font>
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
                <span class="maintitlebar"><font color="#ffffff">Download stable release</font></span>
                </td> </tr>
                <tr>
                  <td class="menus" align="right">
<!--		   <a class="menus" href="http://www.phys.ocean.dal.ca/~kelley/gri-2.6.4-PRE-5.tgz">vsn 2.6.4 pre-release #5</a></br>
-->
		    <b>General tarball</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.6.4.tgz">gri-2.6.4.tgz</a><br>
		    <b>Linux Pre-packaged</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.6.4-1.src.rpm">gri-2.6.4-1.src.rpm</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.6.4-1.i386.rpm">gri-2.6.4-1.i386.rpm</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri_2.6.4-0potato1_i386.deb">gri_2.6.4-0potato1_i386.deb</a><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri_2.6.4-1_i386.deb">gri_2.6.4-1_i386.deb</a><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/science/gri.html">gri-2.6.4 at Debian</A><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/doc/gri-html-doc.html">HTML manual at Debian</A><br>
                    <A class="menus" HREF="http://packages.debian.org/unstable/doc/gri-ps-doc.html">PS manual at Debian</A><br>
		    <b>Solaris Pre-packaged</b><br>
		    <A class="menus" HREF="http://ftp1.sourceforge.net/gri/gri-2.6.0-SunOS5.tar.gz">gri-2.6.0-SunOS6.tar.gz</a><br>

                  </td>
                </tr>
            </tr>

	    <!-- DOWNLOAD UNSTABLE -->
            <tr> 
              <TD align="center" bgcolor="#737b9c">
                <IMG src="./images/blank.gif" height="1" width="1" border=0 alt=""><BR>
                <span class="maintitlebar"><font color="#ffffff">Download unstable release</font></span>
              </td>
            </tr>
            <td class="menus" align="right">
            </td>
            <tr>
              <td class="menus" align="right">
		<b>General tarball</b><br>
                 <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs/gri_2.7.0.orig.tar.gz">Recent CVS sources</A><br>
                <b>Linux Pre-packaged</b><br>
                 <A class="menus" HREF="http://people.debian.org/~psg/gri-cvs.html">CVS Debian Packages</A><br>
              </td>
            </tr>

	    <!-- GENERAL DOWNLOAD AREA -->
            <tr> 
              <TD align="center" bgcolor="#737b9c">
                <IMG src="./images/blank.gif" height="1" width="1" border=0 alt=""><BR>
                <span class="maintitlebar"><font color="#ffffff">Download any release</font></span>
              </td>
            </tr>
            <td class="menus" align="right">
            </td>
            <tr>
              <td class="menus" align="right">
                <a class="menus" href="http://sourceforge.net/project/showfiles.php?group_id=5511">To file download area</a><br>
              </td>
            </tr>
          </table>
      </td>

      <!-- Main Window -->
      <TD VALIGN="top" ALIGN="left" >
      <table align="center" width="95%"><tr><td>
          
Gri is an open-source language for scientific graphics programming.
It is command-driven, as opposed to point/click.  Some users consider
Gri similar to LaTeX, since both provide extensive power as a reward
for tolerating a learning curve.  The output is industry-standard
PostScript as output, suitable for inclusion in other documents.

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
e.g. line widths, colors, fonts, etc.  Greek letters and mathematical
symbols are available in a TeX-like syntax.

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
HREF="http://gri.sourceforge.net/gridoc/html/Version_2_6.html#Version2.6">here</a><br>.


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
        <font size="-1">
        Vsn 2.6.4: 2001-jul-04</br>
        Vsn 2.6.3: broken</br>
        Vsn 2.6.2: 2001-may-20</br>
        Vsn 2.6.1: 2001-may-11</br>
        </font>
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
	 <a class="menus" href="http://www.sourceforge.net/news/?group_id=5511">News Forum</a></b>
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
        <A class="menus" HREF="./gri-cookbook/index.html">Cookbook of Examples</A><br>
        <A class="menus" HREF="./refcard.ps">Reference card</A><br>
        <A class="menus" HREF="./cmdrefcard.ps">Command reference card</A><br>
        <A  class="menus" HREF="http://www2.linuxjournal.com/lj-issues/issue75/3743.html">Article in Linux Journal</A><br>
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
	<A class="menus" HREF="http://www.sourceforge.net/forum/?group_id=5511">Discussion Forums</A><br>
	<A class="menus" HREF="http://sourceforge.net/tracker/?atid=105511&group_id=5511&func=browse">Reporting bugs</A><br>
	<A class="menus" HREF="http://sourceforge.net/tracker/?atid=355511&group_id=5511&func=browse">Requesting features</A><br>
	<A class="menus" HREF="http://www.sourceforge.net/survey/?group_id=5511">Public Surveys</A><br>
	<A class="menus" HREF="http://www.sourceforge.net/mail/?group_id=5511">Mailing Lists</A><br> 
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
	<a class="menus" href="http://sourceforge.net/project/showfiles.php?group_id=5511">File download area</a><br>
        <A class="menus" HREF="http://www.sourceforge.net/patch/?group_id=5511">Submitting patches</A><br>
        <A class="menus" HREF="http://sourceforge.net/pm/task.php?group_project_id=8706&group_id=5511&func=browse">To-do list</A><br>
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
