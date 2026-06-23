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
extern Display *dpy;
// Root window
extern Window root;
// Screen's width in px
extern int screenW;
// Screen's height in px
extern int screenH;

#endif
