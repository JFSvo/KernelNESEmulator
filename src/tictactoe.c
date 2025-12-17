#include <stdint.h>
#include "drivers/vga/vga.h"
#include "tictactoe.h"
#include "kernel.h"
#include "emulator/CPU/emulator.h"

#define CELL_SIZE 40
#define BOARD_X   40   // top-left of the tic tac toe board
#define BOARD_Y   20

#define COLOR_BG      0x00  // background (black)
#define COLOR_X       0x04  // "red" for X
#define COLOR_O       0x01  // "blue" for O
#define COLOR_GRID    0x0F  // white grid lines
#define COLOR_CURSOR  0x0F  // white border for cursor

// game_state:      0x00 = playing     0x0A = X wins (red)    0x0B = O wins (blue)
static uint8_t game_state;

// whose turn? 0x0A = X, 0x0B = O
static uint8_t current_player;

// which cell the cursor is on (0..8)
static int cursor_index;

// simple flag so we lazy-init if needed
static int initialized = 0;



static void draw_rect(int x0, int y0, int w, int h, uint8_t color)
{
    for (int y = 0; y < h; y++) {
        int py = y0 + y;
        for (int x = 0; x < w; x++) {
            int px = x0 + x;
            vga_put_pixel((uint32_t)px, (uint32_t)py, color);
        }
    }
}

// "X" drawing
static void draw_x_symbol(int x0, int y0)
{
    int inner = CELL_SIZE - 6;
    int start = 3;

    for (int i = 0; i < inner; i++) {
        int px1 = x0 + start + i;
        int py1 = y0 + start + i;
        int px2 = x0 + start + i;
        int py2 = y0 + start + (inner - 1 - i);

        vga_put_pixel((uint32_t)px1, (uint32_t)py1, COLOR_X);
        vga_put_pixel((uint32_t)px2, (uint32_t)py2, COLOR_X);
    }
}

// box-shaped "O"
static void draw_o_symbol(int x0, int y0)
{
    int left   = x0 + 3;
    int top    = y0 + 3;
    int right  = x0 + CELL_SIZE - 4;
    int bottom = y0 + CELL_SIZE - 4;

    for (int x = left; x <= right; x++) {
        vga_put_pixel((uint32_t)x, (uint32_t)top,    COLOR_O);
        vga_put_pixel((uint32_t)x, (uint32_t)bottom, COLOR_O);
    }

    for (int y = top; y <= bottom; y++) {
        vga_put_pixel((uint32_t)left,  (uint32_t)y, COLOR_O);
        vga_put_pixel((uint32_t)right, (uint32_t)y, COLOR_O);
    }
}

// Draw Cursor
static void draw_cursor_border(int x0, int y0)
{
    int size = CELL_SIZE / 3;                 // how big the cursor square is
    int offset_x = (CELL_SIZE - size) / 2;    // center inside the cell
    int offset_y = (CELL_SIZE - size) / 2;

    int cx = x0 + offset_x;
    int cy = y0 + offset_y;

    draw_rect(cx, cy, size, size, COLOR_CURSOR);
}

// Draw white grid lines 
static void draw_grid(void)
{
    int board_size = CELL_SIZE * 3;
    int x_start = BOARD_X;
    int y_start = BOARD_Y;
    int x_end   = x_start + board_size;
    int y_end   = y_start + board_size;

    // Vertical lines
    for (int y = y_start; y <= y_end; y++) {
        int x1 = x_start + CELL_SIZE;
        int x2 = x_start + 2 * CELL_SIZE;
        vga_put_pixel((uint32_t)x1, (uint32_t)y, COLOR_GRID);
        vga_put_pixel((uint32_t)x2, (uint32_t)y, COLOR_GRID);
    }

    // Horizontal lines
    for (int x = x_start; x <= x_end; x++) {
        int y1 = y_start + CELL_SIZE;
        int y2 = y_start + 2 * CELL_SIZE;
        vga_put_pixel((uint32_t)x, (uint32_t)y1, COLOR_GRID);
        vga_put_pixel((uint32_t)x, (uint32_t)y2, COLOR_GRID);
    }
}

// Draw all 9 cells
static void draw_board(void)
{
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;

        int x0 = BOARD_X + col * CELL_SIZE;
        int y0 = BOARD_Y + row * CELL_SIZE;

        // Clear inside of the cell 
        draw_rect(x0 + 1, y0 + 1, CELL_SIZE - 2, CELL_SIZE - 2, COLOR_BG);

        // Draw piece if present
        if (emu_read(TTT_BOARD + i) == 0x0A) {
            draw_x_symbol(x0, y0);
        }
        else if (emu_read(TTT_BOARD + i) == 0x0B) {
            draw_o_symbol(x0, y0);
        }

        // Draw cursor border on selected cell
        if (i == cursor_index) {
            draw_cursor_border(x0, y0);
        }
    }
}

void tictactoe_init(void) {
    game_state     = 0x00;  // playing
    current_player = 0x0A;  // X starts
    cursor_index   = 4;     // center cell
    initialized    = 1;
}

// Called by keyboard driver for each scancode
void tictactoe_on_key(uint8_t scancode)
{
    if (!initialized) {
        tictactoe_init();
    }

    // If game is over, only allow 'R' to reset
    if (game_state != 0x00) {
        // 'R' key (scan code 0x13 in set 1)
        if (scancode == 0x13) {
            tictactoe_init();
        }
        return;
    }

    switch (scancode) {

        case 0x11:   // W = up
            if (cursor_index >= 3) {
                cursor_index -= 3;
            }
            break;

        case 0x1F:   // S = down
            if (cursor_index <= 5) {
                cursor_index += 3;
            }
            break;

        case 0x1E:   // A = left
            if ((cursor_index % 3) != 0) {
                cursor_index -= 1;
            }
            break;

        case 0x20:   // D = right
            if ((cursor_index % 3) != 2) {
                cursor_index += 1;
            }
            break;

        // Space or Enter = place piece
        case 0x39:  // Space
        case 0x1C:  // Enter
            if (emu_read(TTT_BOARD + cursor_index) == 0x00) {
                // place current player's piece
                emu_write(TTT_BOARD + cursor_index, current_player);
                current_player = (current_player == 0x0A) ? 0x0B : 0x0A;
            }
            break;

        default:
            // ignore other keys
            break;
    }
}

void tictactoe_check_winner(){
    uint8_t w = emu_read(WIN_BYTE);
    print_hex8(emu_read(WIN_BYTE));
    if (w == 0x0A || w == 0x0B) {
        game_state = w;  // X or O wins
    }     
}

// Called every frame from kernel_main
void tictactoe_update_and_draw(void)
{
    if (!initialized) {
        tictactoe_init();
    }

    tictactoe_check_winner();

    // Winner screens
    if (game_state == 0x0A) {
        vga_clear_screen(COLOR_X);  // full red
        vga_swap_buffers();
        return;
    }
    if (game_state == 0x0B) {
        vga_clear_screen(COLOR_O);  // full blue
        vga_swap_buffers();
        return;
    }

    // Normal board
    vga_clear_screen(COLOR_BG);
    draw_grid();
    draw_board();
    vga_swap_buffers();
}