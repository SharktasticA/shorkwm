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

#include <stdio.h>
#include <X11/Xatom.h>



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

    // Check if the click landed on the close button
    if (ENABLE_TITLE && isOverCloseButton(ev->x, ev->y, client->geo.width))
    {
        // "Ask" program to close
        Atom wmDelete = XInternAtom(DPY, "WM_DELETE_WINDOW", False);
        Atom wmProto = XInternAtom(DPY, "WM_PROTOCOLS", False);

        XEvent closeEv;
        closeEv.xclient.type = ClientMessage;
        closeEv.xclient.window  = client->child;
        closeEv.xclient.message_type = wmProto;
        closeEv.xclient.format = 32;
        closeEv.xclient.data.l[0] = wmDelete;
        closeEv.xclient.data.l[1] = CurrentTime;
        XSendEvent(DPY, client->child, False, NoEventMask, &closeEv);

        return;
    }

    // Bring frame to top
    XRaiseWindow(DPY, client->frame);

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
    XGetWindowAttributes(DPY, client->frame, &wa);
    dragStartX = wa.x;
    dragStartY = wa.y;
    dragPreview = NONE;

    // Grab the pointer so all subsequent PointerMotion and ButtonRelease
    // events are delivered to this frame
    XGrabPointer(DPY, client->frame, False, PointerMotionMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
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
        XSetWindowBorder(DPY, dragFrame, BOR_REST_COL);

    // Release pointer and clear the saved dragged frame
    XUngrabPointer(DPY, CurrentTime);
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
        XConfigureWindow(DPY, client->frame, ev->value_mask, &wc);

    XConfigureWindow(DPY, ev->window, ev->value_mask, &wc);
}

/**
 * On expose event. Used to redraw title bar when this window became visible.
 * @param ev Received XExposeEvent 
 */
void onExpose(XExposeEvent *ev)
{
    if (ev->count != 0)
        return;

    Client *client = findClientByFrame(ev->window);
    if (!client)
        return;

    drawTitleBar(client);
}

/**
 * Used to initiate a child window's Client creation.
 * @param ev Received XMapRequestEvent
 */
void onMapRequest(XMapRequestEvent *ev)
{
    createFrame(ev->window);
    XMapWindow(DPY, ev->window);
}

/**
 * On motion notify event. Used to check when a window's close button is
 * hovered over, or to move a dragged frame and test for a potential snap zone.
 * @param ev Received XMotionEvent
 */
void onMotionNotify(XMotionEvent *ev)
{
    if (dragFrame == None)
    {
        if (ENABLE_TITLE)
        {
            Client *client = findClientByFrame(ev->window);
            if (client)
            {
                // Check if close button's hovered state is to change
                int hovering = isOverCloseButton(ev->x, ev->y, client->geo.width);
                if (hovering != client->closeHover)
                {
                    client->closeHover = hovering;
                    drawTitleBar(client);
                }
            }
        }
        return;
    }

    // Move the frame by the total delta from drag start
    int dx = ev->x_root - dragRootX;
    int dy = ev->y_root - dragRootY;
    XMoveWindow(DPY, dragFrame, dragStartX + dx, dragStartY + dy);

    // Test for potential snap zone and create indicator if needed
    SnapZone zone = getSnapZone(ev->x_root, ev->y_root);
    if (zone != dragPreview)
    {
        dragPreview = zone;
        createSnapIndicator(zone);
        XSetWindowBorder(DPY, dragFrame, (zone != NONE) ? BOR_SNAP_COL : BOR_REST_COL);
    }
}

/**
 * On PropertyNotify event. Used to update a Client's title when WM_NAME
 * changes.
 * @param ev Received XPropertyEvent
 */
void onPropertyNotify(XPropertyEvent *ev)
{
    // Only proceed if WM_NAME has changed
    if (ev->atom != XA_WM_NAME)
        return;

    Client *client = findClientByChild(ev->window);
    if (!client)
        return;

    // Fetch the new WM_NAME and update the Client's name with it
    char *name = NULL;
    if (XFetchName(DPY, client->child, &name) && name)
    {
        snprintf(client->title, sizeof(client->title), "%s", name);
        XFree(name);
    }

    drawTitleBar(client);
}

/**
 * Used to initiate a child window's separation from a Client.
 * @param ev Receieved XUnmapEvent
 */
void onUnmapNotify(XUnmapEvent *ev)
{
    // Ignore UnmapNotify from a XReparentWindow call
    if (ev->event == ROOT)
        return;
    // Ignore UnmapNotify from something we don't manage
    if (!findClientByChild(ev->window))
        return;
    deleteFrame(ev->window);
}
