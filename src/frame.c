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
#include "config.h"
#include "frame.h"

#include <stdio.h>



/**
 * Creates a frame window for the given child window and calls for them to be
 * put in a Client.
 * @param child Child window to frame
 */
void createFrame(Window child)
{
    // If child already has a Client, leave
    if (findClientByChild(child))
        return;

    // Get child's attributions so we can create a frame based on them
    XWindowAttributes wa;
    if (!XGetWindowAttributes(dpy, child, &wa))
        return;
    
    // override_redirect windows are self-managed, so leave if one of them
    if (wa.override_redirect)
        return;

    // Create the frame and tell it what events should be delivered to it
    Window frame = XCreateSimpleWindow(dpy, root, wa.x, wa.y, wa.width, wa.height, BOR_SIZE, BOR_COL, BAK_COL);
    XSelectInput(dpy, frame, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | SubstructureNotifyMask);

    XAddToSaveSet(dpy, child);
    XReparentWindow(dpy, child, frame, 0, 0);
    XMapWindow(dpy, frame);
    XMapWindow(dpy, child);

    addClient(frame, child, (ClientGeometry){ wa.x, wa.y, wa.width, wa.height });
    fprintf(stderr, "Frame: 0x%lx -> 0x%lx\n", child, frame);
}

/**
 * Deletes the given child window's frame window and calls for its Client to
 * be removed.
 * @param child Child window to deframe
 */
void deleteFrame(Window child)
{
    Client *client = findClientByChild(child);
    if (!client)
        return;

    XUnmapWindow(dpy, client->frame);
    XReparentWindow(dpy, child, root, 0, 0);
    XRemoveFromSaveSet(dpy, child);
    XDestroyWindow(dpy, client->frame);

    removeClient(child);
    fprintf(stderr, "Unframe: 0x%lx\n", child);
}
