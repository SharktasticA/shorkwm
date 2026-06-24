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



#ifndef BUTTON
#define BUTTON

#include "client.h"
#include "shorkwm.h"



typedef enum {
    BTN_CAPT_CLOSE,
    BTN_CAPT_MAX
} ButtonType;



void drawButton(ButtonType, Client*, GC, int, int);
void drawCaptButton(ButtonType, Client*, GC, int, int);

#endif 
