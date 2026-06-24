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
#include "shorkwm.h"

#include <math.h>



/**
 * Dispatches the correct button call for the given type.
 * @param type Button type to draw
 * @param client Client to draw on
 * @param gc Graphics context to use
 * @param btnX Button's X position
 * @param btnY Button's Y position
 */
void drawButton(ButtonType type, Client *client, GC gc, int btnX, int btnY)
{
    if (type == BTN_CAPT_CLOSE || type == BTN_CAPT_MAX)
        drawCaptButton(type, client, gc, btnX, btnY);
}

/**
 * Draws a title bar caption button of the given type onto the given Client's
 * frame.
 * @param type Button type to draw
 * @param client Client to draw on
 * @param gc Graphics context to use
 * @param btnX Button's X position
 * @param btnY Button's Y position
 */
void drawCaptButton(ButtonType type, Client *client, GC gc, int btnX, int btnY)
{
    // Resolve whether this button is being hovered over
    int isHovering = 0;
    if (type == BTN_CAPT_CLOSE)
        isHovering = client->closeHover;
    else if (type == BTN_CAPT_MAX)
        isHovering = client->minMaxHover;

    // Select correct hover colour for the given type
    int hoverCol = CAPT_BTN_HOV_COL;
    if (type == BTN_CAPT_CLOSE)
        hoverCol = CLOSE_CAPT_BTN_HOV_COL;

    // Circular button background
    if (CAPT_BTN_RND)
    {
        // Antialiased circular button via XRender
        if (ENABLE_AA)
        {
            // Create XRender picture on frame with the appropriate background
            // colour
            Picture dest = XRenderCreatePicture(DPY, client->frame, XRenderFindVisualFormat(DPY, DefaultVisual(DPY, DefaultScreen(DPY))), 0, NULL);
            int colToUse = isHovering ? hoverCol : CAPT_BTN_BAK_COL;
            XRenderColor xrc = {
                .red   = (((colToUse) >> 16) & 0xFF) * 257,
                .green = ((colToUse >>  8) & 0xFF) * 257,
                .blue  = ((colToUse >>  0) & 0xFF) * 257,
                .alpha = 0xFFFF
            };
            Picture src = XRenderCreateSolidFill(DPY, &xrc);

            // Build polygon of size CAPT_BTN_POLY_PNTS approximating a circle
            double cX = btnX + CAPT_BTN_SIZE / 2.0;
            double cY = btnY + CAPT_BTN_SIZE / 2.0;
            double r = CAPT_BTN_SIZE / 2.0;
            XPointDouble points[CAPT_BTN_POLY_PNTS];
            for (int i = 0; i < CAPT_BTN_POLY_PNTS; i++)
            {
                double angle = 2.0 * M_PI * i / CAPT_BTN_POLY_PNTS;
                points[i].x = cX + r * cos(angle);
                points[i].y = cY + r * sin(angle);
            }

            // Compile the polyon onto the frmae
            XRenderCompositeDoublePoly(DPY, PictOpOver, src, dest, XRenderFindStandardFormat(DPY, PictStandardA8), 0, 0, 0, 0, points, CAPT_BTN_POLY_PNTS, WindingRule);
            XRenderFreePicture(DPY, src);
            XRenderFreePicture(DPY, dest);
        }
        // Non-antialiased circular button with XFillArc
        else
        {
            XSetForeground(DPY, gc, isHovering ? hoverCol : CAPT_BTN_BAK_COL);
            XFillArc(DPY, client->frame, gc, btnX, btnY, CAPT_BTN_SIZE, CAPT_BTN_SIZE, 0, 360 * 64);
        }
    }
    // Rectangular button background
    else
    {
        XSetForeground(DPY, gc, isHovering ? hoverCol : CAPT_BTN_BAK_COL);
        XFillRectangle(DPY, client->frame, gc, btnX, btnY, CAPT_BTN_SIZE, CAPT_BTN_SIZE);
    }

    // Calc size and bounds for button symbol
    int symX1 = btnX + CAPT_BTN_SYM_MAR;
    int symY1 = btnY + CAPT_BTN_SYM_MAR;
    int symX2 = btnX + CAPT_BTN_SIZE - CAPT_BTN_SYM_MAR - (ENABLE_AA || !CAPT_BTN_RND ? 1 : 0);
    int symY2 = btnY + CAPT_BTN_SIZE - CAPT_BTN_SYM_MAR - (ENABLE_AA || !CAPT_BTN_RND ? 1 : 0);

    // Draw an "X" symbol
    if (type == BTN_CAPT_CLOSE)
    {
        XSetForeground(DPY, gc, CLOSE_CAPT_BTN_SYM_COL);
        XDrawLine(DPY, client->frame, gc, symX1, symY1, symX2, symY2);
        XDrawLine(DPY, client->frame, gc, symX2, symY1, symX1, symY2);
    }
    // Draw a maximise or unmaximise symbol
    else if (type == BTN_CAPT_MAX)
    {
        XSetForeground(DPY, gc, CAPT_BTN_SYM_COL);
        // Unmaximise symbol
        if (client->snap == ALL)
        {
            // Top-right box
            XDrawRectangle(DPY, client->frame, gc, symX1 + 1, symY1 - 1, symX2 - symX1, symY2 - symY1);

            // Bottom-left box - one rectangle to cute the other box's corner,
            // other to draw the other box
            XSetForeground(DPY, gc, isHovering ? hoverCol : CAPT_BTN_BAK_COL);
            XFillRectangle(DPY, client->frame, gc, symX1 - 1, symY1 + 1, symX2 - symX1, symY2 - symY1);
            XSetForeground(DPY, gc, CAPT_BTN_SYM_COL);
            XDrawRectangle(DPY, client->frame, gc, symX1 - 1, symY1 + 1, symX2 - symX1, symY2 - symY1);
        }
        // Mmximise symbol
        else
            XDrawRectangle(DPY, client->frame, gc, symX1, symY1, symX2 - symX1, symY2 - symY1);
    }
}
