#include "adventure/adventure.h"
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

void test_adventure_level_defs(void)
{
    TEST("AdventureGetLevelDefs returns valid levels");
    const LevelDef *defs = AdventureGetLevelDefs();
    ASSERT(defs != NULL, "defs is NULL");

    for (int i = 0; i < TOTAL_LEVELS; i++) {
        ASSERT(defs[i].levelNumber == i + 1, "level number mismatch");
        ASSERT(defs[i].goalType == GOAL_SCORE ||
               defs[i].goalType == GOAL_GEMS ||
               defs[i].goalType == GOAL_MIXED_GEMS ||
               defs[i].goalType == GOAL_MIXED_ALL, "valid goalType");

        // Each level should have at least one meaningful target
        bool hasTarget = (defs[i].targetScore > 0) ||
                         (defs[i].targetDiamonds > 0) ||
                         (defs[i].targetEmeralds > 0);
        ASSERT(hasTarget, "level should have a target");

        ASSERT(defs[i].description != NULL, "description should not be NULL");
    }
    PASS();
}

void test_adventure_init_level_with_prefill(void)
{
    TEST("AdventureInitLevel sets up goals and prefill");
    AdventureState adv;
    Board board;
    BoardInit(&board);

    // Init level 3 (GOAL_GEMS, targetDiamonds=3, gemVariance=1, prefillCount=2)
    AdventureInitLevel(&adv, 2, &board);
    ASSERT(adv.currentLevel == 2, "currentLevel should be 2");
    ASSERT(adv.goalDiamonds >= 2, "goalDiamonds should be >= 2 (3 ± 1)");
    ASSERT(adv.goalDiamonds <= 4, "goalDiamonds should be <= 4 (3 ± 1)");
    ASSERT(adv.goalEmeralds == 0, "goalEmeralds should be 0 for gems-only level");

    // Check prefill: board should have some non-empty cells
    int filledCells = 0;
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (board.cells[r][c] != CELL_EMPTY)
                filledCells++;
    ASSERT(filledCells > 0, "board should have pre-filled cells");

    PASS();
}

void test_adventure_init_no_obstacles(void)
{
    TEST("AdventureInitLevel with level 1 (score, no obstacles)");
    AdventureState adv;
    Board board;
    BoardInit(&board);

    AdventureInitLevel(&adv, 0, &board);
    ASSERT(!adv.boardHasObstacles, "level 1 should have no obstacles");
    ASSERT(adv.currentLevel == 0, "currentLevel should be 0");
    ASSERT(adv.goalScore == 500, "level 1 goal score should be 500");

    // All cells should be empty (no prefill for score-only levels)
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            ASSERT(board.cells[r][c] == CELL_EMPTY, "cell should be empty");
    PASS();
}

void test_adventure_update_progress_gems(void)
{
    TEST("AdventureUpdateProgress collects diamonds and emeralds");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 4, &board); // level 5: mixed gems

    AdventureUpdateProgress(&adv, &board, 0, 1, 0, 2, 1);
    ASSERT(adv.collectedDiamonds == 2, "should have 2 diamonds");
    ASSERT(adv.collectedEmeralds == 1, "should have 1 emerald");

    AdventureUpdateProgress(&adv, &board, 0, 1, 0, 1, 1);
    ASSERT(adv.collectedDiamonds == 3, "should have 3 diamonds");
    ASSERT(adv.collectedEmeralds == 2, "should have 2 emeralds");
    PASS();
}

void test_adventure_check_goal_gems(void)
{
    TEST("AdventureCheckGoal returns true when gem goals met");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 2, &board); // level 3: gems only

    ASSERT(!AdventureCheckGoal(&adv), "should not be met yet");

    adv.collectedDiamonds = adv.goalDiamonds;
    ASSERT(AdventureCheckGoal(&adv), "should be met at goal diamonds");
    PASS();
}

void test_adventure_check_goal_mixed_gems(void)
{
    TEST("AdventureCheckGoal for mixed gems (both must be met)");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 4, &board); // level 5: mixed gems

    // Only diamonds met
    adv.collectedDiamonds = adv.goalDiamonds;
    ASSERT(!AdventureCheckGoal(&adv), "emeralds not met yet");

    // Both met
    adv.collectedEmeralds = adv.goalEmeralds;
    ASSERT(AdventureCheckGoal(&adv), "both gems met");
    PASS();
}

void test_adventure_check_failure(void)
{
    TEST("AdventureCheckFailure returns true when no valid moves");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 0, &board); // level 1

    PieceSlot slots[3] = {0};
    // With empty board and no pieces, there are no valid moves
    ASSERT(AdventureCheckFailure(&adv, &board, slots), "should fail with no pieces");

    // Add a valid piece
    slots[0].piece = PieceCreate();
    if (slots[0].piece) {
        ASSERT(!AdventureCheckFailure(&adv, &board, slots), "should not fail with valid piece");
        PieceFree(&slots[0].piece);
    }
    PASS();
}

void test_adventure_progress_save_load(void)
{
    TEST("AdventureSaveProgress and AdventureLoadProgress roundtrip");
    AdventureSaveData save;
    memset(&save, 0, sizeof(save));

    // Set some progress
    save.levels[0].unlocked = true;
    save.levels[0].completed = true;
    save.levels[1].unlocked = true;

    AdventureSaveProgress(&save);

    // Load into a fresh struct
    AdventureSaveData loaded;
    memset(&loaded, 0, sizeof(loaded));
    AdventureLoadProgress(&loaded);

    ASSERT(loaded.levels[0].unlocked, "level 1 should be unlocked");
    ASSERT(loaded.levels[0].completed, "level 1 should be completed");
    ASSERT(loaded.levels[1].unlocked, "level 2 should be unlocked");
    ASSERT(!loaded.levels[1].completed, "level 2 should not be completed");

    // Cleanup
    remove("data/adventure_progress.dat");
    PASS();
}

int main(void)
{
    printf("=== Adventure Tests ===\n\n");

    test_adventure_level_defs();
    test_adventure_init_level_with_prefill();
    test_adventure_init_no_obstacles();
    test_adventure_update_progress_gems();
    test_adventure_check_goal_gems();
    test_adventure_check_goal_mixed_gems();
    test_adventure_check_failure();
    test_adventure_progress_save_load();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}