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



#include "config.h"

#include <stdio.h>



int BAK_COL = 0x000000;

int BOR_REST_COL = 0x3399FF;
int BOR_SNAP_COL = 0x143D66;
int BOR_SIZE = 2;

int CAPT_BTN_BAK_COL = 0x000000;
int CAPT_BTN_HOV_COL = 0xFFD940;
int CAPT_BTN_RND = 1;
int CAPT_BTN_POLY_PNTS = 24;
int CAPT_BTN_SIZE = 12;
int CAPT_BTN_SPACING = 4;
int CAPT_BTN_SYM_MAR = 3;
int CAPT_BTN_SYM_COL = 0xFFFFFF;
int CAPT_BTN_TOP_MAR_EXT = -1;

int CLOSE_CAPT_BTN_HOV_COL = 0xFF4040;
int CLOSE_CAPT_BTN_RIGHT_MAR = 1;
int CLOSE_CAPT_BTN_SYM_COL = 0xFFFFFF;

int ENABLE_AA = 1;
int ENABLE_TITLE = 1;

int SNAP_DET_CORNER_BIAS = 8;
int SNAP_DET_THRES = 16;
int SNAP_INDI_BAK_COL = 0x334455;
int SNAP_INDI_BOR_COL = 0x4488ff;
int SNAP_INDI_BOR_SIZE = 4;

char *TITLE_FONT_NAME = "Sans-10";
int TITLE_HEIGHT = 16;
int TITLE_REST_BAK_COL = 0x3399FF;
int TITLE_REST_TXT_COL = 0xFFFFFF;
int TITLE_SNAP_BAK_COL = 0x143D66;
int TITLE_SNAP_TXT_COL = 0xFFFFFF;



/**
 * Validates configuration variables that cannot simply be any value. If any
 * are found invalid, a suitable replacement or default value is assigned to
 * the offenders.
 */
void validateConfig(void)
{
    if (BOR_SIZE < 0)
    {
        BOR_SIZE = 0;
        fprintf(stderr, "WARNING: BOR_SIZE too low - now set to 0\n");
    }

    if (CAPT_BTN_RND != 0 && CAPT_BTN_RND != 1)
    {
        CAPT_BTN_RND = 1;
        fprintf(stderr, "WARNING: CAPT_BTN_RND non-bolean - now set to 1\n");
    }

    if (CAPT_BTN_POLY_PNTS < 4)
    {
        CAPT_BTN_POLY_PNTS = 4;
        fprintf(stderr, "WARNING: CAPT_BTN_POLY_PNTS too low - now set to 24\n");
    }
    else if (CAPT_BTN_POLY_PNTS > 128)
    {
        CAPT_BTN_POLY_PNTS = 128;
        fprintf(stderr, "WARNING: CAPT_BTN_POLY_PNTS too high - set to 24\n");

    }

    if (CLOSE_CAPT_BTN_RIGHT_MAR < 0)
    {
        CLOSE_CAPT_BTN_RIGHT_MAR = 0;
        fprintf(stderr, "WARNING: CLOSE_CAPT_BTN_RIGHT_MAR too low - now set to 0\n");
    }

    if (ENABLE_AA != 0 && ENABLE_AA != 1)
    {
        ENABLE_AA = 1;
        fprintf(stderr, "WARNING: ENABLE_AA non-bolean - now set to 1\n");
    }

    if (ENABLE_TITLE != 0 && ENABLE_TITLE != 1)
    {
        ENABLE_TITLE = 1;
        fprintf(stderr, "WARNING: ENABLE_TITLE non-bolean - now set to 1\n");
    }

    if (SNAP_DET_CORNER_BIAS <= 0)
    {
        SNAP_DET_CORNER_BIAS = 1;
        fprintf(stderr, "WARNING: SNAP_DET_CORNER_BIAS too low - now set to 1\n");
    }

    if (SNAP_DET_THRES <= 0)
    {
        SNAP_DET_THRES = 1;
        fprintf(stderr, "WARNING: SNAP_DET_THRES too low - now set to 1\n");
    }
}
