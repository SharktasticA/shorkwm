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



#ifndef CLIENT
#define CLIENT

#include "geometry.h"
#include "shorkwm.h"
#include "snap.h"

#include <X11/Xft/Xft.h>



// A window being managed by SHORKWM
typedef struct Client Client;
struct Client {
    // Next client in the list (NULL if tail)
    Client *next;

    // Window title (usually =WM_NAME)
    char title[256];

    // Border/decoration window around child
    Window frame;

    // The application's window itself
    Window child;

    // The window's current location and size
    ClientGeometry geo;

    // A saved copy of a previous location/size (used for restoring a snapped
    // window)
    ClientGeometry savedGeo;

    // Current snap zone (NONE if not snapped; ALL if maximised)
    SnapZone snap;

    // Xft drawing context for title bar text (ENABLE_AA=1)
    XftDraw *xftDraw;

    // Xft font used for title bar text (ENABLE_AA=1)
    XftFont *xftFont;
    
    // Flags if the pointer is currently hovering over the close button
    // (ENABLE_TITLE=1)
    int closeHover;
    
    // Flags if the pointer is currently hovering over the min/max button
    // (ENABLE_TITLE=1)
    int minMaxHover;

    // Flags if this window has keyboard focus
    int focused;
};



// The head of a singly-linked list of windows being managed by SHORKWM
extern Client *CLIENTS;



Client *addClient(Window, Window, ClientGeometry);
Client *findClientByChild(Window);
Client *findClientByFrame(Window);
void removeClient(Window);

#endif 
