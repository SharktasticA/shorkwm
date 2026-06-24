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



#include "client.h"
#include "snap.h"



Window snapIndicator = None;



/**
 * Creates a temporacY window that indicates the snap zone the user is
 * currently holding a window at.
 * @param zone Where we may snap to
 */
void createSnapIndicator(SnapZone zone)
{
    // Erase previous indicator if present
    delSnapIndicator();

    // If we're not in a zone, get out
    if (zone == NONE)
        return;

    // Get potential snap location and dimensions
    ClientGeometry newProps = getSnapGeometry(zone, SNAP_INDI_BOR_SIZE);

    // Create a simple window at the potential snap zone
    snapIndicator = XCreateSimpleWindow(DPY, ROOT, newProps.x, newProps.y, newProps.width, newProps.height, 0, 0, 0);

    // Don't let the WM manage it
    XSetWindowAttributes attr;
    attr.override_redirect = True;

    // Given the window a unique appearance
    attr.background_pixel = SNAP_INDI_BAK_COL;
    attr.border_pixel = SNAP_INDI_BOR_COL;
    XChangeWindowAttributes(DPY, snapIndicator, CWOverrideRedirect | CWBackPixel | CWBorderPixel, &attr);
    XSetWindowBorderWidth(DPY, snapIndicator, SNAP_INDI_BOR_SIZE);

    // Stack it below the held window so it doesn't cover it
    XLowerWindow(DPY, snapIndicator);
    XMapWindow(DPY, snapIndicator);
    XFlush(DPY);
}

/**
 * Destroys the snap indicator window (if present).
 */
void delSnapIndicator(void)
{
    if (snapIndicator != None)
    {
        XDestroyWindow(DPY, snapIndicator);
        snapIndicator = None;
    }
    XFlush(DPY);
}

/**
 * Calculates the window location and size for the given snap zone.
 * @param zone Target snap zone
 * @param borderSize Border size to use in calculations
 * @return Initialised ClientGeometry struct containing new window location and
 *         size values
 */
ClientGeometry getSnapGeometry(SnapZone zone, int borderSize)
{
    ClientGeometry geo = {0};
    int hw = SCREEN_W / 2;
    int hh = SCREEN_H / 2;
    int b2 = borderSize * 2;

    switch (zone)
    {
        case ALL:
            geo.x = 0;
            geo.y = 0;
            geo.width  = SCREEN_W - b2;
            geo.height = SCREEN_H - b2;
            break;
        case NORTH_WEST:
            geo.x = 0;
            geo.y = 0;
            geo.width = hw - b2;
            geo.height = hh - b2;
            break;
        case NORTH:
            geo.x = 0;
            geo.y = 0;
            geo.width = SCREEN_W - b2;
            geo.height = hh - b2;
            break;
        case NORTH_EAST:
            geo.x = hw;
            geo.y = 0;
            geo.width = SCREEN_W - hw - b2;
            geo.height = hh - b2;
            break;
        case EAST:
            geo.x = hw;
            geo.y = 0;
            geo.width = SCREEN_W - hw - b2;
            geo.height = SCREEN_H - b2;
            break;
        case SOUTH_EAST:
            geo.x = hw;
            geo.y = hh;
            geo.width = SCREEN_W - hw - b2;
            geo.height = SCREEN_H - hh - b2;
            break;
        case SOUTH:
            geo.x = 0;
            geo.y = hh;
            geo.width = SCREEN_W - b2;
            geo.height = SCREEN_H - hh - b2;
            break;
        case SOUTH_WEST:
            geo.x = 0;
            geo.y = hh;
            geo.width = hw - b2;
            geo.height = SCREEN_H - hh - b2;
            break;
        case WEST:
            geo.x = 0;
            geo.y = 0;
            geo.width = hw - b2;
            geo.height = SCREEN_H - b2;
            break;
        default:
            break;
    }

    return geo;
}

/**
 * Decides what snap zone the given cursor position is in.
 * @param cX Cursor's X position relative to root
 * @param cY Cursor's Y position relative to root
 * @returns What snap zone the cursor is in
 */
SnapZone getSnapZone(int cX, int cY)
{
    int atNorth = 0, atSouth = 0, atWest = 0, atEast = 0;

    // Checks for corner zones are biased to make them easier to access
    if (cY <= (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS) && cX <= (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS))
    {
        atNorth = 1;
        atWest = 1;
    }
    else if (cY <= (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS) && cX >= SCREEN_W - (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS))
    {
        atNorth = 1;
        atEast = 1;
    }
    else if (cY >= SCREEN_H - (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS) && cX <= (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS))
    {
        atSouth = 1;
        atWest = 1;
    }
    else if (cY >= SCREEN_H - (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS) && cX >= SCREEN_W - (SNAP_DET_THRES * SNAP_DET_CORNER_BIAS))
    {
        atSouth = 1;
        atEast = 1;
    }
    // No bias for the simple directions
    else if (cX <= SNAP_DET_THRES)
        atWest = 1;
    else if (cY <= SNAP_DET_THRES)
        atNorth = 1;
    else if (cX >= SCREEN_W - SNAP_DET_THRES)
        atEast = 1;
    else if (cY >= SCREEN_H - SNAP_DET_THRES)
        atSouth = 1;

    if (atNorth && atWest)
        return NORTH_WEST;
    if (atNorth && atEast)
        return NORTH_EAST;
    if (atSouth && atEast)
        return SOUTH_EAST;
    if (atSouth && atWest)
        return SOUTH_WEST;
    if (atWest)
        return WEST;
    if (atNorth)
        return NORTH;
    if (atEast)
        return EAST;
    if (atSouth)
        return SOUTH;

    return NONE;
}

/**
 * Snaps the given client to the designed snap zone.
 * @param client Client to snap
 * @param zone Where to snap to
 */
void snap(Client *client, SnapZone zone)
{
    // Save pre-snap locations and dimensions
    if (client->snap == NONE)
    {
        XWindowAttributes wa;
        XGetWindowAttributes(DPY, client->frame, &wa);
        client->savedGeo = client->geo;
    }

    // Get and set new location and dimensions
    ClientGeometry newProps = getSnapGeometry(zone, BOR_SIZE);
    XMoveResizeWindow(DPY, client->frame, newProps.x, newProps.y, newProps.width, newProps.height);
    XMoveResizeWindow(DPY, client->child, 0, TITLE_HEIGHT_ACTUAL, newProps.width, newProps.height);
    client->geo = newProps;
    client->snap = zone;

    // Update border to snap colour
    XSetWindowBorder(DPY, client->frame, BOR_SNAP_COL);
}

/**
 * Restores a snapped window to its pre-snap size and either its previous
 * location (snap=ALL) or a new location based on the pointer position (all
 * others).
 * @param client Client to restore to pre-snap
 * @param newX Window's new X position
 * @param newY Window's new Y position
 */
void unsnap(Client *client, int newX, int newY)
{
    // If not snapped, get out
    if (client->snap == NONE)
        return;

    // Reposition and resize window
    if (client->snap == ALL)
        XMoveResizeWindow(DPY, client->frame, client->savedGeo.x, client->savedGeo.y, client->savedGeo.width, client->savedGeo.height + TITLE_HEIGHT_ACTUAL);
    else
        XMoveResizeWindow(DPY, client->frame, newX, newY, client->savedGeo.width, client->savedGeo.height + TITLE_HEIGHT_ACTUAL);
    XMoveResizeWindow(DPY, client->child, 0, TITLE_HEIGHT_ACTUAL, client->savedGeo.width, client->savedGeo.height);

    // Restore pre-snap props
    client->geo = client->savedGeo;
    if (client->snap != ALL)
    {
        client->geo.x = newX;
        client->geo.y = newY;
    }
    client->snap = NONE;

    // Revert to normal border colour
    XSetWindowBorder(DPY, client->frame, BOR_REST_COL);
}
