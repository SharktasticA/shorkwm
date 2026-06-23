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



#include "events.h"
#include "shorkwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static int anotherWMRunning = 0;
Display *dpy = NULL;
Window root = None;
int screenW = 0;
int screenH = 0;



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
        anotherWMRunning = 1;
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

    dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        fprintf(stderr, "Cannot open: %s\n", getenv("DISPLAY") ? getenv("DISPLAY") : "(not set)");
        return 1;
    }

    int scr  = DefaultScreen(dpy);
    root = DefaultRootWindow(dpy);
    screenW = DisplayWidth(dpy, scr);
    screenH = DisplayHeight(dpy, scr);
    fprintf(stderr, "Display: %s (%dx%d)\n", DisplayString(dpy), screenW, screenH);

    XSetErrorHandler(onXError);
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(dpy, False);

    if (anotherWMRunning)
    {
        fprintf(stderr, "ERROR: another window manager is already running\n");
        return 1;
    }

    for (;;)
    {
        XEvent ev;
        XNextEvent(dpy, &ev);

        switch (ev.type)
        {
            case MapRequest:
                onMapRequest(&ev.xmaprequest);
                break;
            case UnmapNotify:
                onUnmapNotify(&ev.xunmap);
                break;
            case ConfigureRequest:
                onConfigureRequest(&ev.xconfigurerequest);
                break;
            case ButtonPress:
                onButtonPress(&ev.xbutton);
                break;
            case ButtonRelease:
                onButtonRelease(&ev.xbutton);
                break;
            case MotionNotify:
                onMotionNotify(&ev.xmotion);
                break;
        }
    }
}
