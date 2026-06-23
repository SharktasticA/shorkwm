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

#include <math.h>
#include <stdio.h>
#include <string.h>
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
    XSetForeground(DPY, gc, TITLE_BAK_COL);
    XFillRectangle(DPY, client->frame, gc, 0, 0, client->geo.width, TITLE_HEIGHT_ACTUAL);

    // Draw Client name
    XSetForeground(DPY, gc, TITLE_TXT_COL);
    XDrawString(DPY, client->frame, gc, 4, TITLE_HEIGHT_ACTUAL - 5, client->name, strlen(client->name));

    // Calc close button position
    int btnX = client->geo.width - CLOSE_BTN_SIZE - CLOSE_BTN_MAR;
    int btnY = (TITLE_HEIGHT_ACTUAL - CLOSE_BTN_SIZE - 1) / 2;

    // Fill close button background
    if (CLOSE_BTN_RND)
    {
        // Smooth circular button
        if (ENABLE_AA)
        {
            Picture dest = XRenderCreatePicture(DPY, client->frame, XRenderFindVisualFormat(DPY, DefaultVisual(DPY, DefaultScreen(DPY))), 0, NULL);
            int colToUse = client->closeHover ? CLOSE_BTN_HOV_COL : CLOSE_BTN_BAK_COL;
            XRenderColor col = {
                .red   = (((colToUse) >> 16) & 0xFF) * 257,
                .green = ((colToUse >>  8) & 0xFF) * 257,
                .blue  = ((colToUse >>  0) & 0xFF) * 257,
                .alpha = 0xFFFF
            };
            Picture src = XRenderCreateSolidFill(DPY, &col);

            // Draw a circle as a 32-point polygon
            int n = 32;
            double cX = btnX + CLOSE_BTN_SIZE / 2.0;
            double cY = btnY + CLOSE_BTN_SIZE / 2.0;
            double r = CLOSE_BTN_SIZE / 2.0;
            XPointDouble points[32];
            for (int i = 0; i < n; i++)
            {
                double angle = 2.0 * M_PI * i / n;
                points[i].x = cX + r * cos(angle);
                points[i].y = cY + r * sin(angle);
            }

            XRenderCompositeDoublePoly(DPY, PictOpOver, src, dest, XRenderFindStandardFormat(DPY, PictStandardA8), 0, 0, 0, 0, points, n, WindingRule);
            XRenderFreePicture(DPY, src);
            XRenderFreePicture(DPY, dest);
        }
        // Jagged but quicker circular button
        else
        {
            XSetForeground(DPY, gc, client->closeHover ? CLOSE_BTN_HOV_COL : CLOSE_BTN_BAK_COL);
            XFillArc(DPY, client->frame, gc, btnX, btnY, CLOSE_BTN_SIZE, CLOSE_BTN_SIZE, 0, 360 * 64);
        }
    }
    // Rectangular button
    else
    {
        XSetForeground(DPY, gc, client->closeHover ? CLOSE_BTN_HOV_COL : CLOSE_BTN_BAK_COL);
        XFillRectangle(DPY, client->frame, gc, btnX, btnY, CLOSE_BTN_SIZE, CLOSE_BTN_SIZE);
    }

    // "X" symbol
    int symX1 = btnX + CLOSE_BTN_SYM_MAR;
    int symY1 = btnY + CLOSE_BTN_SYM_MAR;
    int symX2 = btnX + CLOSE_BTN_SIZE - CLOSE_BTN_SYM_MAR - (ENABLE_AA || !CLOSE_BTN_RND ? 1 : 0);
    int symY2 = btnY + CLOSE_BTN_SIZE - CLOSE_BTN_SYM_MAR - (ENABLE_AA || !CLOSE_BTN_RND ? 1 : 0);
    XSetForeground(DPY, gc, CLOSE_BTN_SYM_COL);
    XDrawLine(DPY, client->frame, gc, symX1, symY1, symX2, symY2);
    XDrawLine(DPY, client->frame, gc, symX2, symY1, symX1, symY2);

    XFreeGC(DPY, gc);
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
    if (!ENABLE_TITLE || TITLE_HEIGHT_ACTUAL == 0)
        return 0;

    // Mirrors where the button was actually placed by drawTitleBar
    int btnX = frameWidth - CLOSE_BTN_SIZE - CLOSE_BTN_MAR;
    int btnY = (TITLE_HEIGHT_ACTUAL - CLOSE_BTN_SIZE - 1) / 2;

    return (frameLocalX >= btnX && frameLocalX <= btnX + CLOSE_BTN_SIZE && frameLocalY >= btnY && frameLocalY <= btnY + CLOSE_BTN_SIZE);
}
