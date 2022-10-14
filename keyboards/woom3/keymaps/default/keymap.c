// Copyright 2022 nyarurato (@nyarurato)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ┌───┬───┬───┐
     * │   │ 0 │   │
     * ├───┼───┼───┤
     * │ 1 │   │ 3 │
     * ├───┼───┼───┤
     * │   │ 2 │   │
     * └───┴───┴───┘
     */
    [0] = LAYOUT(
        KC_P0,   KC_P1,   KC_P2,   KC_P3
    )
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    } 
    return false;
}

void keyboard_post_init_user(void) {
    rgblight_mode_noeeprom(RGBLIGHT_MODE_RAINBOW_MOOD);
    rgblight_enable_noeeprom();

}