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



#include "events.h"
#include "client.h"
#include "frame.h"
#include "snap.h"



// Dragged window frame - used to ensure motion and release events go to the
// window being dragged
static Window dragFrame  = None;
static int dragRootX;
static int dragRootY;
static int dragStartX;
static int dragStartY;
static SnapZone dragPreview = NONE;



/**
 * On mouse button press. Used to begin dragging a Client via its frame window.
 * @param ev Received XButtonEvent
 */
void onButtonPress(XButtonEvent *ev)
{
    // Find the frame's host client
    Client *client = findClientByFrame(ev->window);
    if (!client)
        return;

    // Bring frame to top
    XRaiseWindow(dpy, client->frame);

    // If client is snapped, time to unsnap!
    if (client->snapped)
        restorePreSnap(client, ev->x_root - client->savedGeo.x / 2, ev->y_root - 8);

    // Record the frame being dragged and where the drag started so we calc how
    // far the client moved in onMotionNotify
    dragFrame = client->frame;
    dragRootX = ev->x_root;
    dragRootY = ev->y_root;

    // Record where the frame was so pointer delter is applied to a fixed
    // origin rather than accumulating smol increments
    XWindowAttributes wa;
    XGetWindowAttributes(dpy, client->frame, &wa);
    dragStartX = wa.x;
    dragStartY = wa.y;
    dragPreview = NONE;

    // Grab the pointer so all subsequent PointerMotion and ButtonRelease
    // events are delivered to this frame
    XGrabPointer(dpy, client->frame, False, PointerMotionMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
}

/**
 * On mouse button release. Used to finish a drag and potentially apply a snap.
 * @param ev Received XButtonEvent
 */
void onButtonRelease(XButtonEvent *ev)
{
    // If no active drag, leave
    if (dragFrame == None)
        return;

    // Ensure no snap indicator is active
    delSnapIndicator();

    // Check if we are in a (new) snap zone
    SnapZone zone = getSnapZone(ev->x_root, ev->y_root);
    if (zone != NONE)
    {
        Client *client = findClientByFrame(dragFrame);
        if (client)
            applySnap(client, zone);
    }
    else
        XSetWindowBorder(dpy, dragFrame, BOR_COL);

    // Release pointer and clear the saved dragged frame
    XUngrabPointer(dpy, CurrentTime);
    dragFrame = None;
    dragPreview = NONE;
}

/**
 * On ConfigureRequest received. Allows a child window to change its own
 * geometry.
 * @param ev Received XConfigureRequestEvent
 */
void onConfigureRequest(XConfigureRequestEvent *ev)
{
    XWindowChanges wc;
    wc.x = ev->x;
    wc.y = ev->y;
    wc.width = ev->width;
    wc.height = ev->height;
    wc.border_width = ev->border_width;
    wc.sibling = ev->above;
    wc.stack_mode = ev->detail;

    // If this is one of our Clients, we also need to update the frame window
    Client *client = findClientByChild(ev->window);
    if (client)
        XConfigureWindow(dpy, client->frame, ev->value_mask, &wc);

    XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
}

/**
 * Used to initiate a child window's Client creation.
 * @param ev Received XMapRequestEvent
 */
void onMapRequest(XMapRequestEvent *ev)
{
    createFrame(ev->window);
    XMapWindow(dpy, ev->window);
}

/**
 * On motion notify event. Used to move a dragged frame and test for a
 * potential snap zone.
 * @param ev Received XMotionEvent
 */
void onMotionNotify(XMotionEvent *ev)
{
    if (dragFrame == None)
        return;

    // Move the frame by the total delta from drag start
    int dx = ev->x_root - dragRootX;
    int dy = ev->y_root - dragRootY;
    XMoveWindow(dpy, dragFrame, dragStartX + dx, dragStartY + dy);

    // Test for potential snap zone and create indicator if needed
    SnapZone zone = getSnapZone(ev->x_root, ev->y_root);
    if (zone != dragPreview)
    {
        dragPreview = zone;
        createSnapIndicator(zone);
        XSetWindowBorder(dpy, dragFrame, (zone != NONE) ? SNAP_COL : BOR_COL);
    }
}

/**
 * Used to initiate a child window's separation from a Client.
 * @param ev Receieved XUnmapEvent
 */
void onUnmapNotify(XUnmapEvent *ev)
{
    // Ignore UnmapNotify from a XReparentWindow call
    if (ev->event == root)
        return;
    // Ignore UnmapNotify from something we don't manage
    if (!findClientByChild(ev->window))
        return;
    deleteFrame(ev->window);
}
