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

#include "shorkwm.h"



// A window's location and size 
typedef struct {
    int x;
    int y;
    int width;
    int height;
} ClientGeometry;

// A window being managed by SHORKWM
typedef struct Client Client;
struct Client {
    // Next client in the list (NULL if tail)
    Client *next;

    // Window's snap (usually WM_NAME)
    char name[256];

    // Border/decoration window around child
    Window frame;

    // The application's window itself
    Window child;

    // The window's current location and size
    ClientGeometry geo;

    // A saved copy of a previous location/size (used for restoring a snapped
    // window)
    ClientGeometry savedGeo;

    // Flags if the window is currently snapped
    int snapped;

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
