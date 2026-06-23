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
 * Snaps the given client to the designed snap zone.
 * @param c Client to snap
 * @param zone Where to snap to
 */
void applySnap(Client *c, SnapZone zone)
{
    // Save pre-snap locations and dimensions
    if (!c->snapped)
    {
        XWindowAttributes wa;
        XGetWindowAttributes(dpy, c->frame, &wa);
        c->savedGeo = c->geo;
    }

    // Get and set new location and dimensions
    ClientGeometry newProps = getSnapGeometry(zone);
    XMoveResizeWindow(
        dpy, c->frame,
        newProps.x,
        newProps.y,
        newProps.width,
        newProps.height
    );
    XMoveResizeWindow(
        dpy, c->child, 0, 0,
        newProps.width,
        newProps.height
    );
    c->geo = newProps;
    c->snapped = 1;

    // Update border to snap colour
    XSetWindowBorder(dpy, c->frame, SNAP_COL);
}

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
    ClientGeometry newProps = getSnapGeometry(zone);

    // Create a simple window at the potential snap zone
    snapIndicator = XCreateSimpleWindow(
        dpy, root,
        newProps.x,
        newProps.y,
        newProps.width,
        newProps.height,
        0, 0, 0);

    // Don't let the WM manage it
    XSetWindowAttributes attr;
    attr.override_redirect = True;

    // Given the window a unique appearance
    attr.background_pixel = SNAP_INDI_BAK;
    attr.border_pixel = SNAP_INDI_BOR;
    XChangeWindowAttributes(dpy, snapIndicator, CWOverrideRedirect | CWBackPixel | CWBorderPixel, &attr);
    XSetWindowBorderWidth(dpy, snapIndicator, 3);

    // Stack it below the held window so it doesn't cover it
    XLowerWindow(dpy, snapIndicator);
    XMapWindow(dpy, snapIndicator);
    XFlush(dpy);
}

/**
 * Destroys the snap indicator window (if present).
 */
void delSnapIndicator(void)
{
    if (snapIndicator != None)
    {
        XDestroyWindow(dpy, snapIndicator);
        snapIndicator = None;
    }
    XFlush(dpy);
}

/**
 * Calculates the window location and size for the given snap zone.
 * @param zone Target snap zone
 * @return Initialised ClientGeometry struct containing new window location and
 *         size values
 */
ClientGeometry getSnapGeometry(SnapZone zone)
{
    ClientGeometry geo = {0};
    int hw = screenW / 2;
    int hh = screenH / 2;
    int b2 = BOR_SIZE * 2;

    switch (zone)
    {
        case NORTH_WEST:
            geo.x = 0;
            geo.y = 0;
            geo.width = hw - b2;
            geo.height = hh - b2;
            break;
        case NORTH:
            geo.x = 0;
            geo.y = 0;
            geo.width = screenW - b2;
            geo.height = hh - b2;
            break;
        case NORTH_EAST:
            geo.x = hw;
            geo.y = 0;
            geo.width = screenW - hw - b2;
            geo.height = hh - b2;
            break;
        case EAST:
            geo.x = hw;
            geo.y = 0;
            geo.width = screenW - hw - b2;
            geo.height = screenH - b2;
            break;
        case SOUTH_EAST:
            geo.x = hw;
            geo.y = hh;
            geo.width = screenW - hw - b2;
            geo.height = screenH - hh - b2;
            break;
        case SOUTH:
            geo.x = 0;
            geo.y = hh;
            geo.width = screenW - b2;
            geo.height = screenH - hh - b2;
            break;
        case SOUTH_WEST:
            geo.x = 0;
            geo.y = hh;
            geo.width = hw - b2;
            geo.height = screenH - hh - b2;
            break;
        case WEST:
            geo.x = 0;
            geo.y = 0;
            geo.width = hw - b2;
            geo.height = screenH - b2;
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
    if (cY <= (SNAP_THRES * SNAP_CORNER_BIAS) && cX <= (SNAP_THRES * SNAP_CORNER_BIAS))
    {
        atNorth = 1;
        atWest = 1;
    }
    else if (cY <= (SNAP_THRES * SNAP_CORNER_BIAS) && cX >= screenW - (SNAP_THRES * SNAP_CORNER_BIAS))
    {
        atNorth = 1;
        atEast = 1;
    }
    else if (cY >= screenH - (SNAP_THRES * SNAP_CORNER_BIAS) && cX <= (SNAP_THRES * SNAP_CORNER_BIAS))
    {
        atSouth = 1;
        atWest = 1;
    }
    else if (cY >= screenH - (SNAP_THRES * SNAP_CORNER_BIAS) && cX >= screenW - (SNAP_THRES * SNAP_CORNER_BIAS))
    {
        atSouth = 1;
        atEast = 1;
    }
    // No bias for the simple directions
    else if (cX <= SNAP_THRES)
        atWest = 1;
    else if (cY <= SNAP_THRES)
        atNorth = 1;
    else if (cX >= screenW - SNAP_THRES)
        atEast = 1;
    else if (cY >= screenH - SNAP_THRES)
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
 * Restores a snapped window to its pre-snap geometry and repositioning it at
 * the new given location.
 * @param c Client to restore to pre-snap
 * @param newX Window's new X position
 * @param newY Window's new Y position
 */
void restorePreSnap(Client *c, int newX, int newY)
{
    // If not snapped, get out
    if (!c->snapped) return;

    // Reposition and resize window
    XMoveResizeWindow(dpy, c->frame, newX, newY, c->savedGeo.width, c->savedGeo.height);
    XMoveResizeWindow(dpy, c->child, 0, 0, c->savedGeo.width, c->savedGeo.height);

    // Restore pre-snap props
    c->geo = c->savedGeo;
    c->snapped = 0;

    // Revert to normal border colour
    XSetWindowBorder(dpy, c->frame, BOR_COL);
}
