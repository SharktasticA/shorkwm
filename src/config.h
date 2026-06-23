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

// Window border colour when not snapped
extern int BOR_REST_COL;
// Window border colour when snapped
extern int BOR_SNAP_COL;
// Window border size
extern int BOR_SIZE;

// Close button background colour
extern int CLOSE_BTN_BAK_COL;
// Close button hover colour
extern int CLOSE_BTN_HOV_COL;
// Close button margin from right
extern int CLOSE_BTN_MAR;
// Close button rounded flag (1=rectangle, 0=circle)
extern int CLOSE_BTN_RND;
// Close button width/height
extern int CLOSE_BTN_SIZE; 
// Close button symbol colour
extern int CLOSE_BTN_SYM_COL;
// Close button symbol margin from left
extern int CLOSE_BTN_SYM_MAR;

// Enables antialiasing/smoother shapes and text (1=y, 0=n)
extern int ENABLE_AA;
// Enables window title bars (1=y, 0=n)
extern int ENABLE_TITLE;

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

// Window title bar font name (when ENABLE_AA=1)
extern char *TITLE_FONT_NAME;
// Window title bar height
extern int TITLE_HEIGHT;
// Window title bar background colour when not snapped
extern int TITLE_REST_BAK_COL;
// Window title bar text colour when not snapped
extern int TITLE_REST_TXT_COL;
// Window title bar background colour when snapped
extern int TITLE_SNAP_BAK_COL;
// Window title bar text colour when snapped
extern int TITLE_SNAP_TXT_COL;

#endif
