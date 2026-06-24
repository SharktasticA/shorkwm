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



#include "button.h"
#include "client.h"
#include "config.h"
#include "frame.h"

#include <stdio.h>
#include <string.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>



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
    if (!XGetWindowAttributes(DPY, child, &wa))
        return;

    // override_redirect windows are self-managed, so leave if one of them
    if (wa.override_redirect)
        return;

    // Create the frame and tell it what events should be delivered to it
    Window frame = XCreateSimpleWindow(DPY, ROOT, wa.x, wa.y, wa.width, wa.height + TITLE_HEIGHT_ACTUAL, BOR_SIZE, BOR_REST_COL, BAK_COL);
    XSelectInput(DPY, frame, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | SubstructureNotifyMask | ExposureMask);

    XAddToSaveSet(DPY, child);
    XReparentWindow(DPY, child, frame, 0, TITLE_HEIGHT_ACTUAL);
    XSelectInput(DPY, child, PropertyChangeMask);
    XMapWindow(DPY, frame);
    XMapWindow(DPY, child);

    // Draw the initial title bar
    Client *client = addClient(frame, child, (ClientGeometry){ wa.x, wa.y, wa.width, wa.height });
    if (client)
    {
        if (ENABLE_AA)
        {
            client->xftDraw = XftDrawCreate(DPY, client->frame, DefaultVisual(DPY, DefaultScreen(DPY)), DefaultColormap(DPY, DefaultScreen(DPY)));
            client->xftFont = XftFontOpenName(DPY, DefaultScreen(DPY), TITLE_FONT_NAME);
        }

        char *name = NULL;
        if (XFetchName(DPY, child, &name) && name)
        {
            snprintf(client->title, sizeof(client->title), "%s", name);
            XFree(name);
        }

        drawTitleBar(client);
    }

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

    XUnmapWindow(DPY, client->frame);
    XReparentWindow(DPY, child, ROOT, 0, 0);
    XRemoveFromSaveSet(DPY, child);
    XDestroyWindow(DPY, client->frame);

    removeClient(child);
    fprintf(stderr, "Unframe: 0x%lx\n", child);
}

/**
 * Draws a title bar string for the given Client's frame window.
 * @param client Client to draw a title bar for
 */
void drawTitleBar(Client *client)
{
    if (!ENABLE_TITLE || TITLE_HEIGHT_ACTUAL == 0)
        return;

    // Create a temporary GC for drawing
    GC gc = XCreateGC(DPY, client->frame, 0, NULL);

    // Fill the title bar strip
    XSetForeground(DPY, gc, (client->snap != NONE) ? TITLE_SNAP_BAK_COL : TITLE_REST_BAK_COL);
    XFillRectangle(DPY, client->frame, gc, 0, 0, client->geo.width, TITLE_HEIGHT_ACTUAL);

    // Draw Client name
    if (ENABLE_AA)
    {
        XftColor col;
        XRenderColor xrc = {
            .red   = ((TITLE_REST_TXT_COL >> 16) & 0xFF) * 257,
            .green = ((TITLE_REST_TXT_COL >>  8) & 0xFF) * 257,
            .blue  = ((TITLE_REST_TXT_COL >>  0) & 0xFF) * 257,
            .alpha = 0xFFFF
        };
        XftColorAllocValue(DPY, DefaultVisual(DPY, DefaultScreen(DPY)), DefaultColormap(DPY, DefaultScreen(DPY)), &xrc, &col);
        XftDrawStringUtf8(client->xftDraw, &col, client->xftFont, 4, TITLE_HEIGHT_ACTUAL - 5, (FcChar8*)client->title, strlen(client->title));
        XftColorFree(DPY, DefaultVisual(DPY, DefaultScreen(DPY)), DefaultColormap(DPY, DefaultScreen(DPY)), &col);
    }
    else
    {
        XSetForeground(DPY, gc, (client->snap != NONE) ? TITLE_SNAP_TXT_COL : TITLE_REST_TXT_COL);
        XDrawString(DPY, client->frame, gc, 4, TITLE_HEIGHT_ACTUAL - 5, client->title, strlen(client->title));
    }

    // Calc close button position and draw
    int btnX = client->geo.width - CAPT_BTN_SIZE - CLOSE_CAPT_BTN_RIGHT_MAR;
    int btnY = ((TITLE_HEIGHT_ACTUAL - CAPT_BTN_SIZE) / 2) + CAPT_BTN_TOP_MAR_EXT;
    drawButton(BTN_CAPT_CLOSE, client, gc, btnX, btnY);

    // Calc maximise/unmaximise button position and draw
    btnX -= CAPT_BTN_SPACING + CAPT_BTN_SIZE;
    drawButton(BTN_CAPT_MAX, client, gc, btnX, btnY);

    XFreeGC(DPY, gc);
}

/**
 * Checks if the pointer is hovering over where the given title bar caption
 * button resides.
 * @param Which caption button to check
 * @param frameLocalX Pointer's X position relative to frame's top-left
 * @param frameLocalY Pointer's Y position relative to frame's top-left
 * @param frameWidth Frame width in pixels
 * @return 1 if over close button; 0 if not
 */
int isOverCaptButton(ButtonType type, int frameLocalX, int frameLocalY, int frameWidth)
{
    if (!ENABLE_TITLE || TITLE_HEIGHT_ACTUAL == 0)
        return 0;

    // Mirrors where the button was actually placed by drawTitleBar
    int btnX = frameWidth - CAPT_BTN_SIZE - CLOSE_CAPT_BTN_RIGHT_MAR;
    int btnY = (TITLE_HEIGHT_ACTUAL - CAPT_BTN_SIZE - 1) / 2;

    if (type == BTN_CAPT_MAX)
        btnX -= CAPT_BTN_SPACING + CAPT_BTN_SIZE;

    return (frameLocalX >= btnX && frameLocalX <= btnX + CAPT_BTN_SIZE && frameLocalY >= btnY && frameLocalY <= btnY + CAPT_BTN_SIZE);
}
