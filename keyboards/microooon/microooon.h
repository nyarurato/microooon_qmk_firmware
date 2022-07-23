// Copyright 2022 nyarurato (@nyarurato)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

/* This is a shortcut to help you visually see your layout.
 *
 * The first section contains all of the arguments representing the physical
 * layout of the board and position of the keys.
 *
 * The second converts the arguments into a two-dimensional array which
 * represents the switch matrix.
 */
#define LAYOUT( \
	\
    s11a, s12a, s13a, s14a, s15a, s21a, s22a, s23a, s24a, s25a, s31a, s32a,\
	s33a, s34a, s35a, s41a, s42a, s43a, s44a, s45a, s51a, s52a, s53a, s54a,\
	s55a, s11b, s12b, s13b, s14b, s15b, s21b, s22b, s23b, s24b, s25b, s31b,\
	s32b, s33b, s34b, s35b, s41b, s42b, s43b, s44b, s45b, s51b, s52b, s53b,\
    s54b, s55b, s16a, s26a, s36a, s46a\
) { \
	{s11a, s21a, s31a, s41a, s51a, s11b, s21b, s31b, s41b, s51b, }, \
	{s12a, s22a, s32a, s42a, s52a, s12b, s22b, s32b, s42b, s52b, }, \
	{s13a, s23a, s33a, s43a, s53a, s13b, s23b, s33b, s43b, s53b, }, \
	{s14a, s24a, s34a, s44a, s54a, s14b, s24b, s34b, s44b, s54b, }, \
	{s15a, s25a, s35a, s45a, s55a, s15b, s25b, s35b, s45b, s55b, }, \
	{s16a, s26a, s36a, s46a, KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,KC_NO,}\
}
