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

void test_piece_create_free(void)
{
    TEST("PieceCreate allocates and PieceFree frees");
    Piece *p = PieceCreate();
    ASSERT(p != NULL, "PieceCreate returned NULL");
    ASSERT(p->width > 0 && p->width <= MAX_PIECE_SIZE, "invalid width");
    ASSERT(p->height > 0 && p->height <= MAX_PIECE_SIZE, "invalid height");
    ASSERT(p->colorIndex >= 1 && p->colorIndex <= 7, "invalid color index");

    // Check shape has at least one filled cell
    bool hasFilled = false;
    for (int r = 0; r < p->height && !hasFilled; r++)
        for (int c = 0; c < p->width && !hasFilled; c++)
            if (p->shape[r][c] == 1) hasFilled = true;
    ASSERT(hasFilled, "piece has no filled cells");

    PieceFree(&p);
    ASSERT(p == NULL, "pointer should be NULL after free");
    PASS();
}

void test_piece_definitions(void)
{
    TEST("GetPieceDefinitions returns valid definitions");
    const PieceDef *defs = GetPieceDefinitions();
    ASSERT(defs != NULL, "defs is NULL");

    // Count definitions (should be > 0)
    ASSERT(PIECE_DEF_COUNT > 0, "PIECE_DEF_COUNT should be > 0");
    for (int i = 0; i < PIECE_DEF_COUNT; i++) {
        ASSERT(defs[i].width > 0 && defs[i].width <= MAX_PIECE_SIZE, "invalid def width");
        ASSERT(defs[i].height > 0 && defs[i].height <= MAX_PIECE_SIZE, "invalid def height");
        // Verify shape has at least one filled cell
        bool hasFilled = false;
        for (int r = 0; r < defs[i].height && !hasFilled; r++)
            for (int c = 0; c < defs[i].width && !hasFilled; c++)
                if (defs[i].shape[r][c] == 1) hasFilled = true;
        ASSERT(hasFilled, "definition has no filled cells");
    }
    PASS();
}

void test_generate_random_pieces(void)
{
    TEST("GenerateRandomPieces fills all 3 slots");
    PieceSlot slots[3];
    memset(slots, 0, sizeof(slots));

    GenerateRandomPieces(slots, 600.0f, 480.0f);

    for (int i = 0; i < 3; i++) {
        ASSERT(slots[i].piece != NULL, "slot should have a piece");
        ASSERT(slots[i].posX >= 0, "posX should be set");
        ASSERT(slots[i].posY >= 0, "posY should be set");
    }

    // Cleanup
    for (int i = 0; i < 3; i++)
        PieceFree(&slots[i].piece);
    PASS();
}

void test_all_slots_empty(void)
{
    TEST("AllSlotsEmpty correctly detects empty slots");
    PieceSlot slots[3];
    memset(slots, 0, sizeof(slots));

    ASSERT(AllSlotsEmpty(slots), "all NULL slots should be empty");

    slots[0].piece = PieceCreate();
    ASSERT(!AllSlotsEmpty(slots), "one filled slot should not be empty");

    PieceFree(&slots[0].piece);
    ASSERT(AllSlotsEmpty(slots), "after freeing, should be empty again");
    PASS();
}

int main(void)
{
    printf("=== Piece Tests ===\n\n");

    test_piece_create_free();
    test_piece_definitions();
    test_generate_random_pieces();
    test_all_slots_empty();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}