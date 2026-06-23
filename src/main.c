/*
    ######################################################
    ##               SHORK WINDOW MANAGER               ##
    ######################################################
    ## A lightweight window manager capable of snapping ##
    ## & tiling                                         ##
    ######################################################
    ## Licence: GNU GENERAL PUBLIC LICENSE Version 3    ##
    ######################################################
    ## Kali (sharktastica.co.uk)                        ##
    ######################################################
*/



static const char *VERSION = "1.0-pt1";



#include "config.h"
#include "events.h"
#include "shorkwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static int ANOTHER_WM_RUNNING = 0;
Display *DPY = NULL;
Window ROOT = None;
int SCREEN_W = 0;
int SCREEN_H = 0;
int TITLE_HEIGHT_ACTUAL = 0;



/**
 * Xlib error callback called by the X server upon a non-fatal error.
 * @param d
 * @param err Received XErrorEvent
 * @return Always 0
 */
static int onXError(Display *d, XErrorEvent *err)
{
    (void)d;
    if (err->error_code == BadAccess)
    {
        ANOTHER_WM_RUNNING = 1;
        return 0;
    }
    char buffer[256];
    XGetErrorText(d, err->error_code, buffer, sizeof(buffer));
    fprintf(stderr, "X: %s (request=%d)\n", buffer, err->request_code);
    return 0;
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--version") == 0))
        {
            printf("SHORKWM %s\n", VERSION);
            return 0;
        }
    }

    DPY = XOpenDisplay(NULL);
    if (!DPY)
    {
        fprintf(stderr, "Cannot open: %s\n", getenv("DISPLAY") ? getenv("DISPLAY") : "(not set)");
        return 1;
    }

    int scr  = DefaultScreen(DPY);
    ROOT = DefaultRootWindow(DPY);
    SCREEN_W = DisplayWidth(DPY, scr);
    SCREEN_H = DisplayHeight(DPY, scr);
    fprintf(stderr, "Display: %s (%dx%d)\n", DisplayString(DPY), SCREEN_W, SCREEN_H);

    // Validate boolean config values to ensure they have a good default if
    // incorrectly set
    if (CLOSE_BTN_RND != 0 && CLOSE_BTN_RND != 1)
        CLOSE_BTN_RND = 1;
    if (ENABLE_AA != 0 && ENABLE_AA != 1)
        ENABLE_AA = 1;
    if (ENABLE_TITLE != 0 && ENABLE_TITLE != 1)
        ENABLE_TITLE = 1;

    TITLE_HEIGHT_ACTUAL = ENABLE_TITLE ? TITLE_HEIGHT : 0;

    XSetErrorHandler(onXError);
    XSelectInput(DPY, ROOT, SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(DPY, False);

    if (ANOTHER_WM_RUNNING)
    {
        fprintf(stderr, "ERROR: another window manager is already running\n");
        return 1;
    }

    for (;;)
    {
        XEvent ev;
        XNextEvent(DPY, &ev);

        switch (ev.type)
        {
            case ButtonPress:
                onButtonPress(&ev.xbutton);
                break;
            case ButtonRelease:
                onButtonRelease(&ev.xbutton);
                break;
            case ConfigureRequest:
                onConfigureRequest(&ev.xconfigurerequest);
                break;
            case Expose:
                onExpose(&ev.xexpose);
                break;
            case MapRequest:
                onMapRequest(&ev.xmaprequest);
                break;
            case MotionNotify:
                onMotionNotify(&ev.xmotion);
                break;
            case PropertyNotify:
                onPropertyNotify(&ev.xproperty);
                break;
            case UnmapNotify:
                onUnmapNotify(&ev.xunmap);
                break;
        }
    }
}
