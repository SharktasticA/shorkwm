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



#ifndef SNAP
#define SNAP

#include "config.h"
#include "geometry.h"
#include "shorkwm.h"



typedef struct Client Client;

// Possible window snap zones
typedef enum
{
    NONE,
    ALL,
    NORTH_WEST,
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST
} SnapZone;



// Window to display when indicating a potential snap zone
extern Window snapIndicator;



void createSnapIndicator(SnapZone);
void delSnapIndicator(void);
ClientGeometry getSnapGeometry(SnapZone, int);
SnapZone getSnapZone(int, int);
void snap(Client*, SnapZone);
void unsnap(Client*, int, int);

#endif
