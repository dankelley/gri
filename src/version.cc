// NB: _gri_number will be checked against line 1 of gri.cmd
#ifdef OSX_BUNDLE
char _gri_number[] = PACKAGE_VERSION;
#else
// Don't ask me why, but I used to do
//        #define stringify(x) # x
//        char _gri_number[] = stringify(VERSION);
// here.  I think I was messing with the OSX bundle release,
// but whatever I was doing, it sems not to be needed.  I'll
// just leave it here for a while, though.
// DEK 2005-12-18
//
char _gri_number[] = PACKAGE_VERSION;
#endif
char _gri_release_time[] = "2006 Jan 6"; // Australia Day
char _gri_date[] = "2006";
