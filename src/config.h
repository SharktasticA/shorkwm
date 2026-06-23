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



#ifndef CONFIG
#define CONFIG

// Generic background colour
extern int BAK_COL;

// Window border size
extern int BOR_SIZE;
// Window border colour when not snapped
extern int BOR_REST_COL;
// Window border colour when snapped
extern int BOR_SNAP_COL;

// Close button background colour
extern int CLOSE_BTN_BAK_COL;
// Close button hover colour
extern int CLOSE_BTN_HOV_COL;
// Close button padding
extern int CLOSE_BTN_PAD;
// Close button width/height
extern int CLOSE_BTN_SIZE; 
// Close button symbol colour
extern int CLOSE_BTN_SYM_COL;

// Snap zone detection corner bias
extern int SNAP_DET_CORNER_BIAS;
// Snap zone detection threshold
extern int SNAP_DET_THRES;
// Snap indicator background colour
extern int SNAP_INDI_BAK_COL;
// Snap indiactor border colour
extern int SNAP_INDI_BOR_COL;
// Snap indicator border size
extern int SNAP_INDI_BOR_SIZE;

// Title bar height
extern int TITLE_HEIGHT;
// Title bar background colour
extern int TITLE_BAK_COL;
// Title bar enabled (1=y, 0=n)
extern int TITLE_ENABLED;
// Title bar text colour
extern int TITLE_TXT_COL;

#endif
