/*
    ######################################################
    ##               SHORK WINDOW MANAGER               ##
    ######################################################
    ## TODO                                             ##
    ######################################################
    ## Licence: GNU GENERAL PUBLIC LICENSE Version 3    ##
    ######################################################
    ## Kali (sharktastica.co.uk)                        ##
    ######################################################
*/



#ifndef SHORKWM
#define SHORKWM

#include <X11/Xlib.h>



// Connection to the X server
extern Display *DPY;
// Root window
extern Window ROOT;
// Screen's width in px
extern int SCREEN_W;
// Screen's height in px
extern int SCREEN_H;
// Title bar height to use (can be 0 if ENABLE_TITLE=0)
extern int TITLE_HEIGHT_ACTUAL;

#endif
