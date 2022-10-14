// Copyright 2022 nyarurato (@nyarurato)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

#define VENDOR_ID    0x4E59 //NY
#define PRODUCT_ID   0x5257 //RU+2
#define DEVICE_VER   0x0001
#define MANUFACTURER Nyaru
#define PRODUCT      woom3

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT


#define MATRIX_ROWS 1
#define MATRIX_COLS 4

#define ENCODERS_PAD_A { GP0 }
#define ENCODERS_PAD_B { GP1 }

#define RGB_DI_PIN GP15
#ifdef RGB_DI_PIN
#    define RGBLED_NUM 12
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#    define RGBLIGHT_LIMIT_VAL 80
#    define RGBLIGHT_SLEEP
/*== all animations enable ==*/
#define RGBLIGHT_ANIMATIONS
#define RGBLIGHT_EFFECT_BREATHE_MAX 80
#define RGBLIGHT_EFFECT_TWINKLE_LIFE 100
#endif

#define DIRECT_PINS {{GP3, GP4, GP5, GP6}}