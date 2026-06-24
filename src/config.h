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
// Window border size (>= 0)
extern int BOR_SIZE;

// Caption button background colour
extern int CAPT_BTN_BAK_COL;
// Caption button hover colour
extern int CAPT_BTN_HOV_COL;
//Caption button no. of polygon points (when ENABLE_AA=1, 4-128)
extern int CAPT_BTN_POLY_PNTS;
// Caption button rounded flag (1=rectangle, 0=circle)
extern int CAPT_BTN_RND;
// Caption button width/height
extern int CAPT_BTN_SIZE;
// Caption button spacing
extern int CAPT_BTN_SPACING;
// Caption button symbol surrounding margin
extern int CAPT_BTN_SYM_MAR;
// Caption button symbol colour
extern int CAPT_BTN_SYM_COL;
// Caption button extra top margin
extern int CAPT_BTN_TOP_MAR_EXT;

// Close caption button hover colour
extern int CLOSE_CAPT_BTN_HOV_COL;
// Close caption button margin from right (>=0)
extern int CLOSE_CAPT_BTN_RIGHT_MAR;
// Close caption button symbol colour
extern int CLOSE_CAPT_BTN_SYM_COL;

// Enables antialiasing/smoother shapes and text (1=y, 0=n)
extern int ENABLE_AA;
// Enables window title bars (1=y, 0=n)
extern int ENABLE_TITLE;

// Snap zone detection corner bias (>= 1)
extern int SNAP_DET_CORNER_BIAS;
// Snap zone detection threshold (>= 1)
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



void validateConfig(void);

#endif
