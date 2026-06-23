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
#include <string.h>



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
    Window frame = XCreateSimpleWindow(dpy, root, wa.x, wa.y, wa.width, wa.height + TITLE_HEIGHT, BOR_SIZE, BOR_COL, BAK_COL);
    XSelectInput(dpy, frame, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | SubstructureNotifyMask | ExposureMask);

    XAddToSaveSet(dpy, child);
    XReparentWindow(dpy, child, frame, 0, TITLE_HEIGHT);
    XSelectInput(dpy, child, PropertyChangeMask);
    XMapWindow(dpy, frame);
    XMapWindow(dpy, child);

    // Draw the initial title bar
    Client *client = addClient(frame, child, (ClientGeometry){ wa.x, wa.y, wa.width, wa.height });
    if (client)
        drawTitleBar(client);

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

/**
 * Draws a title bar string for the given Client's frame window.
 * @param client Client to draw a title bar for
 */
void drawTitleBar(Client *client)
{
    // Create a temporary GC for drawing
    GC gc = XCreateGC(dpy, client->frame, 0, NULL);

    // Fill the title bar strip
    XSetForeground(dpy, gc, TITLE_BAK_COL);
    XFillRectangle(dpy, client->frame, gc, 0, 0, client->geo.width, TITLE_HEIGHT);

    // Draw Client name
    XSetForeground(dpy, gc, TITLE_TXT_COL);
    XDrawString(dpy, client->frame, gc, 4, TITLE_HEIGHT - 5, client->name, strlen(client->name));

    // Calc close button position
    int btnX = client->geo.width - CLOSE_BTN_SIZE - CLOSE_BTN_PAD;
    int btnY = (TITLE_HEIGHT - CLOSE_BTN_SIZE) / 2;

    // Fill close button background
    XSetForeground(dpy, gc, client->closeHover ? CLOSE_BTN_HOV_COL : CLOSE_BTN_BAK_COL);
    XFillRectangle(dpy, client->frame, gc, btnX, btnY, CLOSE_BTN_SIZE, CLOSE_BTN_SIZE);

    // Draw the "X" close symbol
    XSetForeground(dpy, gc, CLOSE_BTN_SYM_COL);
    XDrawLine(dpy, client->frame, gc, btnX + 3, btnY + 3, btnX + CLOSE_BTN_SIZE - 3, btnY + CLOSE_BTN_SIZE - 3);
    XDrawLine(dpy, client->frame, gc, btnX + CLOSE_BTN_SIZE - 3, btnY + 3, btnX + 3, btnY + CLOSE_BTN_SIZE - 3);

    XFreeGC(dpy, gc);
}

/**
 * Checks if the pointer is hovering over where a title bar close button would
 * be.
 * @param frameLocalX Pointer's X position relative to frame's top-left
 * @param frameLocalY Pointer's Y position relative to frame's top-left
 * @param frameWidth Frame width in pixels
 * @return 1 if over close button; 0 if not
 */
int isOverCloseButton(int frameLocalX, int frameLocalY, int frameWidth)
{
    // Mirrors where the button was actually placed by drawTitleBar
    int btnX = frameWidth - CLOSE_BTN_SIZE - CLOSE_BTN_PAD;
    int btnY = (TITLE_HEIGHT - CLOSE_BTN_SIZE) / 2;

    return (frameLocalX >= btnX && frameLocalX <= btnX + CLOSE_BTN_SIZE && frameLocalY >= btnY && frameLocalY <= btnY + CLOSE_BTN_SIZE);
}
