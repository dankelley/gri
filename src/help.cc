/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include        <string>
#include        <stdio.h>
#include        "gr.hh"
#include        "extern.hh"
#include        "files.hh"


bool            give_overall_help_message(void);
bool            give_help_on_topic(void);

bool
helpCmd()
{
	if (_nword == 1) {
		(void) give_overall_help_message();
		return true;
	}
	// Give help on topic
	if (_nword == 3 && !strcmp(_word[1], "-")) {
		give_help_on_topic();
		return true;
	} else {
		// Must be `help item ...'
		int             i = 0, cmd;
		bool            found = false;
		std::string tmpname_file(tmp_file_name());
		FILE           *fp;
		if (!(fp = fopen(tmpname_file.c_str(), "w"))) {
			printf("buffer filename is '%s'\n",tmpname_file.c_str());
			err("Sorry, error opening buffer-file for `help'");
			return false;
		}
		// Figure out what command, and give help for it.
		while (*(_cmdLine + i) != ' ')
			i++;
		while (*(_cmdLine + i) == ' ')
			i++;
		strcat(_cmdLine, " *");
		for (cmd = 0; cmd < _num_command; cmd++) {
			if (same_syntax(_cmdLine + i, _command[cmd].syntax, 1)) {
				found = true;
				fprintf(fp, "%s\n", _command[cmd].help);
			}
		}
		if (found) {
			fclose(fp);
			more_file_to_terminal(tmpname_file.c_str());
			delete_file(tmpname_file.c_str());
			return true;
		} else {
			fclose(fp);
			delete_file(tmpname_file.c_str());
			err("Sorry, can't understand 'help' request.");
			give_overall_help_message();
			return false;
		}
	}
}

bool
give_overall_help_message()
{
	ShowStr("\
Type `help' followed by a command-name:\n\
    assert      cd            close         convert\n\
    create      debug         delete        differentiate\n\
    draw        expecting     filter        flip\n\
    get         help          if            ignore\n\
    input       insert        interpolate   list\n\
    ls          mask          move          new\n\
    open        pwd           query         quit\n\
    read        regress       reorder       rescale\n\
    resize      return        rewind        set\n\
    show        skip          sleep         smooth\n\
    source      sprintf       state         superuser\n\
    system      write\n\
Or type `help -' followed by a topic from this list:\n\
    example     extending     files         math\n\
    strings     synonyms      variables     manual\n\
To exit, type `quit'.\n\
");
	return true;
}

bool
give_help_on_topic()
{
	FILE           *fp;
	std::string          tmpname_file(tmp_file_name());
	if (!(fp = fopen(tmpname_file.c_str(), "w"))) {
		err("Sorry, error opening buffer-file for `help -'");
		return false;
	} else if (!strcmp(_word[2], "example")) {
		fprintf(fp, "\
// Example of plot with 2 curves on it:\n\
// Note: the `//' symbol means rest of line is a comment\n\
open filename1   // open 1st data-file\n\
read columns x y // read columnar xy data\n\
draw curve       // draw 1st data curve\n\
close            // close 1st data-file\n\
open filename2   // open 2nd data-file\n\
read columns x y\n\
close\n\
set dash         // make this line dashed\n\
draw curve       // superimpose 2nd curve\n\
quit             // end of plot\n\
");
		fclose(fp);
	} else if (!strcmp(_word[2], "extending")) {
		fprintf(fp, "\
Extending gri by defining new commands:\n\
You can define new commands as in this example:\n\n\
\n\
`New Command'\n\
This is help for this new command.  Any lines between the new\n\
command name and the opening brace are made into the help for the\n\
command.  Note that the new command has the name New Command, and\n\
can be invoked just by naming it.  By convention, you should name\n\
new commands with UPPER CASE as the first characters, to distinguish\n\
them from normal commands.  Also by convention, the body of \n\
the command should be indented 4 spaces.\n\
{\n\
    show \"You've called `New Command'\"\n\
}\n\
");
		fclose(fp);
	} else if (!strcmp(_word[2], "files")) {
		fprintf(fp, "\
Data-file operations:\n\
open\n\
close\n\
show next line\n\
read columns\n\
read grid x\n\
read grid y\n\
read grid data\n\
skip forward\n\
skip backward\n\
");
		fclose(fp);
	} else if (!strcmp(_word[2], "math")) {
		fprintf(fp, "\
Math operations:\n\
Simple format:\n\
 `item += #', `item -= #', `item *= #', `item /= #'\n\
where item is a column or a variable. For variables only (eg, `.a.'),\n\
there is also the form\n\
 `.variable. = #'\n\
In all forms, # may be a number:\n\
 `x += 2'\n\
or a variable:\n\
 `x += .offset.'\n\
or a reverse-polish expression in numbers/variables:\n\
 `x += { rpn .phase. 30 + 10 / sin }\n\
");
		fclose(fp);
	} else if (!strcmp(_word[2], "strings")) {
		fprintf(fp, "\
MATH SYMBOLS: get these by imbedding in dollar signs as in TeX.\n\
Example \"$\\alpha$\".\n\
SUPERSCRIPTS & SUBSCRIPTS:  Within this `math mode', get superscripts with\n\
^c for a single character (c) or ^{ccc} for several characters (ccc);\n\
for subscripts use _c and _{ccc}.\n\
\n\
SPECIAL CHARACTERS: You get \\ using \\\\, ^ using \\^, and _ using \\^.\n\
Outside math mode, things are different; ^ and _ are just ordinary\n\
characters with no special meaning, and \\ is used for synonyms.\n\
\n\
SYNONYMS: Thus you may define \\file as a filename with \n\
        `query \\filename \"give filename\" {\"input.dat\"}'\n\
\n\
Then whenever \\filename occurs in a string, gri will substitute \n\
whatever string you've supplied.  Finally, you may at any time get gri \n\
to print a $ by using \\$.\n\
");
		fclose(fp);
	} else if (!strcmp(_word[2], "synonyms")) {
		fprintf(fp, "\
Synonyms store character strings.  You may use them in place of any\n\
normal Gri command word, and also within quoted strings.\n\
\n\
Examples of assigning to synonyms:\n\
  \\synonym = \"Text to store in synonym\"        // store that text\n\
  \\synonym = system \"date\"                     // store the date\n\
  // (Note that above form limits \\synonym to 8192 characters)\n\
  query \\synonym \"Prompt string\" (\"default\")   // get user input\n\
\n\
Examples of using synonyms:\n\
  // Making filenames be flexible and interactive:\n\
  open \\file\n\
  draw title \"Data from file \\file\"\n\
  // Extracting words from within synonyms\n\
  \\sentence = \"This synonym has several words\"\n\
  \\first_word = word 0 from \"\\sentence\"\n\
  \\second_word = word 1 from \"\\sentence\"\n\
");
		fclose(fp);
	} else if (!strcmp(_word[2], "variables")) {
		fprintf(fp, "\
Variables store numbers.  You may use them in place of any numbers\n\
in any Gri command.\n\
\n\
Examples of assigning to variables:\n\
  .time. = 10\n\
  .time. += 1\n\
  .time. = {rpn .time. 2 *}\n\
  read .time.\n\
\n\
Examples of using variables:\n\
  read columns .number. x y\n\
  draw label \"hello\" at .xcm. .ycm.\n\
  sprintf \\title \"x is %%f and y is %%f (centimeters)\" .x. .y.\n\
  draw label \"\\title\" at .x. .y. cm\n\
");
		fclose(fp);
	} else if (word_is(2, "manual")) {
		fprintf(fp, "\
The Gri manual should be available online through the unix command\n\
   info gri\n\
and this same manual should be available inside emacs.  Peter Galbraith\n\
emacs editing mode for Gri yields easy access to this manual, with the\n\
ability to look up help on the command where the editing cursor resides.\n\
\n\
Normally a PostScript manual is also available.  See your system manager.\n\
\n\
Also, a world-wide-web manual is available; as of November 1994, this\n\
is at the location http://www.cs.dal.ca/users/kelley/gri/gri1.html.\n\
");
		fclose(fp);
	} else {
		err("Sorry, no help on that topic");
		give_overall_help_message();
		return false;
	}
	more_file_to_terminal(tmpname_file.c_str());
	delete_file(tmpname_file.c_str());
	return true;
}
