#include "core/board.h"
#include "core/piece.h"
#include "core/config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { printf("  TEST: %s ... ", name); } while(0)
#define PASS() do { printf("PASSED\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("FAILED: %s\n", msg); tests_failed++; } while(0)
#define ASSERT(cond, msg) do { if (!(cond)) { FAIL(msg); return; } } while(0)

static Piece *make_test_piece(int w, int h, int color)
{
    Piece *p = (Piece *)malloc(sizeof(Piece));
    memset(p->shape, 0, sizeof(p->shape));
    p->width = w;
    p->height = h;
    p->colorIndex = color;
    for (int r = 0; r < h; r++)
        for (int c = 0; c < w; c++)
            p->shape[r][c] = 1;
    return p;
}

void test_board_init(void)
{
    TEST("BoardInit clears all cells");
    Board b;
    BoardInit(&b);
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            ASSERT(b.cells[r][c] == CELL_EMPTY, "cell not empty after init");
    PASS();
}

void test_board_place_and_clear(void)
{
    TEST("BoardPlace + BoardClearLines (single row)");
    Board b;
    BoardInit(&b);

    // Fill the first row completely
    Piece *p = make_test_piece(GRID_SIZE, 1, 1);
    ASSERT(BoardCanPlace(&b, p, 0, 0), "should be able to place");
    BoardPlace(&b, p, 0, 0);
    PieceFree(&p);

    // Check cells are filled
    for (int c = 0; c < GRID_SIZE; c++)
        ASSERT(b.cells[0][c] != CELL_EMPTY, "cell should be filled");

    // Clear lines
    bool cleared[GRID_SIZE][GRID_SIZE] = {false};
    int lines = BoardClearLines(&b, cleared, NULL, NULL);
    ASSERT(lines == 1, "should clear 1 line");

    // Check row is now empty
    for (int c = 0; c < GRID_SIZE; c++)
        ASSERT(b.cells[0][c] == CELL_EMPTY, "cell should be empty after clear");

    PASS();
}

void test_board_cannot_place_outside(void)
{
    TEST("BoardCanPlace rejects out-of-bounds placement");
    Board b;
    BoardInit(&b);
    Piece *p = make_test_piece(2, 2, 1);

    ASSERT(!BoardCanPlace(&b, p, -1, 0), "negative row should fail");
    ASSERT(!BoardCanPlace(&b, p, 0, -1), "negative col should fail");
    ASSERT(!BoardCanPlace(&b, p, GRID_SIZE, 0), "row beyond grid should fail");
    ASSERT(!BoardCanPlace(&b, p, 0, GRID_SIZE), "col beyond grid should fail");
    ASSERT(!BoardCanPlace(&b, p, GRID_SIZE - 1, GRID_SIZE - 1), "overflow should fail");

    PieceFree(&p);
    PASS();
}

void test_board_has_valid_move(void)
{
    TEST("BoardHasValidMove returns true when moves exist");
    Board b;
    BoardInit(&b);

    PieceSlot slots[3];
    memset(slots, 0, sizeof(slots));
    slots[0].piece = make_test_piece(1, 1, 1);

    ASSERT(BoardHasValidMove(&b, slots), "should have valid move for 1x1 piece");

    PieceFree(&slots[0].piece);
    PASS();
}

void test_board_clear_column(void)
{
    TEST("BoardClearLines clears a full column");
    Board b;
    BoardInit(&b);

    // Fill the first column completely
    for (int r = 0; r < GRID_SIZE; r++) {
        Piece *p = make_test_piece(1, 1, 1);
        BoardPlace(&b, p, r, 0);
        PieceFree(&p);
    }

    bool cleared[GRID_SIZE][GRID_SIZE] = {false};
    int lines = BoardClearLines(&b, cleared, NULL, NULL);
    ASSERT(lines == 1, "should clear 1 column");

    for (int r = 0; r < GRID_SIZE; r++)
        ASSERT(b.cells[r][0] == CELL_EMPTY, "column cell should be empty");

    PASS();
}

void test_board_clear_multiple_lines(void)
{
    TEST("BoardClearLines clears multiple lines at once");
    Board b;
    BoardInit(&b);

    // Fill first 3 rows completely
    for (int r = 0; r < 3; r++) {
        Piece *p = make_test_piece(GRID_SIZE, 1, 1);
        BoardPlace(&b, p, r, 0);
        PieceFree(&p);
    }

    bool cleared[GRID_SIZE][GRID_SIZE] = {false};
    int lines = BoardClearLines(&b, cleared, NULL, NULL);
    ASSERT(lines == 3, "should clear 3 rows");

    PASS();
}

void test_board_cannot_place_overlap(void)
{
    TEST("BoardCanPlace rejects overlapping placement");
    Board b;
    BoardInit(&b);

    Piece *p1 = make_test_piece(1, 1, 1);
    ASSERT(BoardCanPlace(&b, p1, 0, 0), "first placement should work");
    BoardPlace(&b, p1, 0, 0);

    Piece *p2 = make_test_piece(1, 1, 1);
    ASSERT(!BoardCanPlace(&b, p2, 0, 0), "overlapping placement should fail");

    PieceFree(&p1);
    PieceFree(&p2);
    PASS();
}

int main(void)
{
    printf("=== Board Tests ===\n\n");

    test_board_init();
    test_board_place_and_clear();
    test_board_cannot_place_outside();
    test_board_has_valid_move();
    test_board_clear_column();
    test_board_clear_multiple_lines();
    test_board_cannot_place_overlap();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}