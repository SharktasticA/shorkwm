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



#ifndef FRAME
#define FRAME

#include "shorkwm.h"



void createFrame(Window);
void deleteFrame(Window);
void drawTitleBar(Client*);
int isOverCloseButton(int, int, int);

#endif
