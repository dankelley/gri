/*
 * Define flags for superuser
 *
 * To get combination flags, simply add them; thus a flag of 5 gives
 * flags 1 and 4 together.
 * 
 * NOTE: for flag meanings, see startup.cc, where strings are defined; 
 * just search for e.g. FLAG_SYN in that file.
 */


#define FLAG_SYN  0x0001	//   1 print cmdline before/after sub synonyms
#define FLAG_RPN  0x0002	//   2 print cmdline before/after sub rpn
#define FLAG_NEW  0x0004	//   4 print new commands being defined
#define FLAG_SYS  0x0008	//   8 print system commands before passing
#define FLAG_FLOW 0x0010	//  16 indicate program flow
#define FLAG_AUT2 0x0080	// 128 flag for author's use
#define FLAG_AUT1 0x0100	// 256 flag for author's use
