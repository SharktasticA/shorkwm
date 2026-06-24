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

#include <stdlib.h>
#include <string.h>



Client  *CLIENTS  = NULL;



/**
 * Creates a new Client and prepends it to the CLIENTS list.
 * @param frame New client's frame window
 * @param child New client's child window
 * @param startGeo New client's starting location and size
 * @return The new Client
 */
Client *addClient(Window frame, Window child, ClientGeometry startGeo)
{
    Client *client = calloc(1, sizeof(Client));
    if (!client)
        return NULL;

    client->title[0] = '\0';
    client->frame = frame;
    client->child = child;
    client->geo = startGeo;
    client->snap = NONE;
    client->focused = 0;

    // Prepend to CLIENTS list
    client->next = CLIENTS;
    CLIENTS = client;

    return client;
}

/**
 * Gets the client that owns the given child window.
 * @param win Child window to find
 * @return Client that owns the given child window
 */
Client *findClientByChild(Window win)
{
    for (Client *client = CLIENTS; client; client = client->next)
        if (client->child == win)
            return client;
    return NULL;
}

/**
 * Gets the client that owns the given frame window.
 * @param win Frame window to find
 * @return Client that owns the given frame window
 */
Client *findClientByFrame(Window win)
{
    for (Client *client = CLIENTS; client; client = client->next)
        if (client->frame == win)
            return client;
    return NULL;
}

/**
 * Unlinks and free the client that owns the given child window.
 * @param child Child window to find
 */
void removeClient(Window child)
{
    Client **pp = &CLIENTS;
    while (*pp)
    {
        Client *client = *pp;
        // We look for a child not frame because DestroyNotify & UnmapNotify
        // events arrive on the child XID
        if (client->child == child)
        {
            *pp = client->next;
            free(client);
            return;
        }
        pp = &client->next;
    }
}
