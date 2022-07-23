/*
Copyright 2012-2018 Jun Wako, Jack Humbert, Yiancar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "quantum.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"

#ifdef SPLIT_KEYBOARD
#    include "split_common/split_util.h"
#    include "split_common/transactions.h"

#    define ROWS_PER_HAND (MATRIX_ROWS / 2)
#else
#    define ROWS_PER_HAND (MATRIX_ROWS)
#endif

#ifdef DIRECT_PINS_RIGHT
#    define SPLIT_MUTABLE
#else
#    define SPLIT_MUTABLE const
#endif
#ifdef MATRIX_ROW_PINS_RIGHT
#    define SPLIT_MUTABLE_ROW
#else
#    define SPLIT_MUTABLE_ROW const
#endif
#ifdef MATRIX_COL_PINS_RIGHT
#    define SPLIT_MUTABLE_COL
#else
#    define SPLIT_MUTABLE_COL const
#endif

#ifdef DIRECT_PINS
static SPLIT_MUTABLE pin_t direct_pins[MATRIX_ROWS][MATRIX_COLS] = DIRECT_PINS;
#elif (DIODE_DIRECTION == ROW2COL) || (DIODE_DIRECTION == COL2ROW)
#    ifdef MATRIX_ROW_PINS
static SPLIT_MUTABLE_ROW pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
#    endif  // MATRIX_ROW_PINS
#    ifdef MATRIX_COL_PINS
static SPLIT_MUTABLE_COL pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
#    endif  // MATRIX_COL_PINS
#endif


// for duprex matrix
#define COL_PIN_NUMS (MATRIX_COLS / 2)

/* matrix state(1:on, 0:off) */
extern matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
extern matrix_row_t matrix[MATRIX_ROWS];      // debounced values

// user-defined overridable functions
__attribute__((weak)) void matrix_init_pins(void);
__attribute__((weak)) void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row);
__attribute__((weak)) void matrix_read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col);

static inline void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

static inline void setPinInputHigh_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON { setPinInputHigh(pin); }
}

static inline uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return readPin(pin);
    } else {
        return 1;
    }
}

// matrix code

static bool select_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void unselect_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinInputHigh_atomic(pin);
    }
}

static void unselect_rows(void) {
    for (uint8_t x = 0; x < ROWS_PER_HAND; x++) {
        unselect_row(x);
    }
}


static bool select_col(uint8_t col) {
    pin_t pin = col_pins[col];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void unselect_col(uint8_t col) {
    pin_t pin = col_pins[col];
    if (pin != NO_PIN) {
        setPinInputHigh_atomic(pin);
    }
}

static void unselect_cols(void) {
    for (uint8_t x = 0; x < COL_PIN_NUMS; x++) {
        unselect_col(x);
    }
}


__attribute__((weak)) void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    if (!select_row(current_row)) {  // Select row
        return;                      // skip NO_PIN row
    }
    matrix_output_select_delay();
    
    for (uint8_t col_index = 0; col_index < COL_PIN_NUMS; col_index++) {
        uint8_t pin_state = readMatrixPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        //current_row_value |= pin_state ? 0 : row_shifter;

        current_row_value |= pin_state ? 0 : (MATRIX_ROW_SHIFTER << col_index);
        /*
        if (readMatrixPin(col_pins[col_index]) == 0) {
            // Pin LO, set col bit
            current_matrix[current_row] |= (MATRIX_ROW_SHIFTER << col_index);
            key_pressed = true;
        } else {
            // Pin HI, clear col bit
            current_matrix[current_row] &= ~(MATRIX_ROW_SHIFTER << col_index);
        }
        */
    }

    // Unselect row
    unselect_row(current_row);
    matrix_output_unselect_delay(current_row, current_row_value != 0);  // wait for all Col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

__attribute__((weak)) void matrix_read_rows_on_col(matrix_row_t current_matrix[], uint8_t current_col) {
    bool key_pressed = false;

    // Select col
    if (!select_col(current_col)) {  // select col
        return;                      // skip NO_PIN col
    }
    matrix_output_select_delay();

    // For each row...
    for (uint8_t row_index = 0; row_index < ROWS_PER_HAND; row_index++ ) {
        // Check row pin state
        if (readMatrixPin(row_pins[row_index]) == 0) {
            // Pin LO, set col bit
            current_matrix[row_index] |= (MATRIX_ROW_SHIFTER << (current_col + COL_PIN_NUMS));
            key_pressed = true;
        } else {
            // Pin HI, clear col bit
            current_matrix[row_index] &= ~(MATRIX_ROW_SHIFTER << (current_col + COL_PIN_NUMS));
        }
    }

    // Unselect col
    unselect_col(current_col);
    matrix_output_unselect_delay(current_col, key_pressed);  // wait for all Row signals to go HIGH
}

__attribute__((weak)) void matrix_init_pins(void) {
    unselect_rows();
    unselect_cols();
    
    for (uint8_t x = 0; x < COL_PIN_NUMS; x++) {
        if (col_pins[x] != NO_PIN) {
            setPinInputHigh_atomic(col_pins[x]);
        }
    }

    
    for (uint8_t x = 0; x < ROWS_PER_HAND; x++) {
        if (row_pins[x] != NO_PIN) {
            setPinInputHigh_atomic(row_pins[x]);
        }
    }

}

void matrix_init(void) {//必要
    // initialize key pins
    matrix_init_pins();

    // initialize matrix state: all keys off
    memset(matrix, 0, sizeof(matrix));
    memset(raw_matrix, 0, sizeof(raw_matrix));

    debounce_init(ROWS_PER_HAND);

    matrix_init_quantum();

}

uint8_t matrix_scan(void) {//必要
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

    // Set row, read cols
    for (uint8_t current_row = 0; current_row < ROWS_PER_HAND; current_row++) {
        matrix_read_cols_on_row(curr_matrix, current_row);
    }

    // Set col, read rows
    for (uint8_t current_col = 0; current_col < COL_PIN_NUMS; current_col++) {
        matrix_read_rows_on_col(curr_matrix, current_col);
    }

    bool changed = memcmp(raw_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(raw_matrix, curr_matrix, sizeof(curr_matrix));
    
    debounce(raw_matrix, matrix, ROWS_PER_HAND, changed);
    matrix_scan_quantum();

    return (uint8_t)changed;
}
