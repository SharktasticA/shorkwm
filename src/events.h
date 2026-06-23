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



#ifndef EVENTS
#define EVENTS

#include "shorkwm.h"



void onButtonPress(XButtonEvent*);
void onButtonRelease(XButtonEvent*);
void onConfigureRequest(XConfigureRequestEvent*);
void onMapRequest(XMapRequestEvent*);
void onMotionNotify(XMotionEvent*);
void onUnmapNotify(XUnmapEvent*);

#endif
