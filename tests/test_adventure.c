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
        ASSERT(defs[i].goalValue > 0, "goalValue should be > 0");
        ASSERT(defs[i].pieceLimit > 0, "pieceLimit should be > 0");
        ASSERT(defs[i].description != NULL, "description should not be NULL");
    }
    PASS();
}

void test_adventure_init_level(void)
{
    TEST("AdventureInitLevel clears board and sets obstacles");
    AdventureState adv;
    Board board;
    BoardInit(&board);

    // Init level 8 (has ice obstacles)
    AdventureInitLevel(&adv, 7, &board);
    ASSERT(adv.currentLevel == 7, "currentLevel should be 7");
    ASSERT(adv.boardHasObstacles, "level 8 should have obstacles");
    ASSERT(adv.piecesUsed == 0, "piecesUsed should start at 0");
    ASSERT(adv.goalProgress == 0, "goalProgress should start at 0");

    // Check obstacles were placed
    const LevelDef *defs = AdventureGetLevelDefs();
    int obstacleCount = defs[7].obstacleCount;
    ASSERT(obstacleCount > 0, "level 8 should have obstacles");

    // Check ice cells have under-colors
    for (int i = 0; i < obstacleCount; i++) {
        int r = defs[7].obstacles[i].row;
        int c = defs[7].obstacles[i].col;
        if (defs[7].isIce[i]) {
            ASSERT(board.cells[r][c] == CELL_ICE, "ice cell should be CELL_ICE");
            int under = AdventureGetIceUnderColor(&adv, r, c);
            ASSERT(under >= 1 && under <= 7, "ice under-color should be 1-7");
        } else {
            ASSERT(board.cells[r][c] == CELL_STONE, "stone cell should be CELL_STONE");
        }
    }
    PASS();
}

void test_adventure_init_no_obstacles(void)
{
    TEST("AdventureInitLevel with no obstacles");
    AdventureState adv;
    Board board;
    BoardInit(&board);

    AdventureInitLevel(&adv, 0, &board);
    ASSERT(!adv.boardHasObstacles, "level 1 should have no obstacles");
    ASSERT(adv.currentLevel == 0, "currentLevel should be 0");

    // All cells should be empty
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            ASSERT(board.cells[r][c] == CELL_EMPTY, "cell should be empty");
    PASS();
}

void test_adventure_update_progress_clear_lines(void)
{
    TEST("AdventureUpdateProgress with GOAL_CLEAR_LINES");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 0, &board); // level 1: clear 3 lines

    AdventureUpdateProgress(&adv, &board, 2, 1, 0);
    ASSERT(adv.goalProgress == 2, "should have 2 lines cleared");

    AdventureUpdateProgress(&adv, &board, 1, 1, 0);
    ASSERT(adv.goalProgress == 3, "should have 3 lines cleared");
    PASS();
}

void test_adventure_update_progress_score(void)
{
    TEST("AdventureUpdateProgress with GOAL_SCORE");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 2, &board); // level 3: score 500

    AdventureUpdateProgress(&adv, &board, 0, 1, 200);
    ASSERT(adv.goalProgress == 200, "should have 200 points");

    AdventureUpdateProgress(&adv, &board, 0, 1, 300);
    ASSERT(adv.goalProgress == 500, "should have 500 points");
    PASS();
}

void test_adventure_update_progress_combo(void)
{
    TEST("AdventureUpdateProgress with GOAL_COMBO");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 3, &board); // level 4: 2 combos

    // Combo < 2 should not count
    AdventureUpdateProgress(&adv, &board, 1, 1, 0);
    ASSERT(adv.goalProgress == 0, "combo 1 should not count");

    // Combo >= 2 should count
    AdventureUpdateProgress(&adv, &board, 2, 2, 0);
    ASSERT(adv.goalProgress == 1, "combo 2 should count as 1");

    AdventureUpdateProgress(&adv, &board, 3, 3, 0);
    ASSERT(adv.goalProgress == 2, "combo 3 should count as 2");
    PASS();
}

void test_adventure_check_goal(void)
{
    TEST("AdventureCheckGoal returns true when goal met");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 0, &board); // level 1: clear 3 lines

    ASSERT(!AdventureCheckGoal(&adv), "should not be met yet");

    adv.goalProgress = 3;
    ASSERT(AdventureCheckGoal(&adv), "should be met at 3 lines");
    PASS();
}

void test_adventure_check_failure(void)
{
    TEST("AdventureCheckFailure returns true when pieces exhausted without goal");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 0, &board); // level 1: clear 3 lines, 20 pieces

    ASSERT(!AdventureCheckFailure(&adv), "should not fail at start");

    adv.piecesUsed = 20; // piece limit
    ASSERT(AdventureCheckFailure(&adv), "should fail if goal not met at limit");

    adv.goalProgress = 3; // now goal is met
    ASSERT(!AdventureCheckFailure(&adv), "should not fail if goal is met");
    PASS();
}

void test_adventure_calc_stars(void)
{
    TEST("AdventureCalcStars returns correct star count");
    AdventureState adv;
    Board board;
    BoardInit(&board);
    AdventureInitLevel(&adv, 0, &board); // level 1: clear 3 lines

    adv.goalProgress = 0;
    ASSERT(AdventureCalcStars(&adv) == 0, "0 progress = 0 stars");

    adv.goalProgress = 3;  // 100% = STAR_1_THRESHOLD
    ASSERT(AdventureCalcStars(&adv) == 1, "100% = 1 star");

    adv.goalProgress = 5;  // 166% > STAR_2_THRESHOLD (1.5)
    ASSERT(AdventureCalcStars(&adv) == 2, "166% = 2 stars");

    adv.goalProgress = 6;  // 200% = STAR_3_THRESHOLD
    ASSERT(AdventureCalcStars(&adv) == 3, "200% = 3 stars");
    PASS();
}

void test_adventure_progress_save_load(void)
{
    TEST("AdventureSaveProgress and AdventureLoadProgress roundtrip");
    AdventureSaveData save;
    memset(&save, 0, sizeof(save));

    // Set some progress
    save.levels[0].unlocked = true;
    save.levels[0].bestStars = 3;
    save.levels[0].completed = true;
    save.levels[1].unlocked = true;
    save.levels[1].bestStars = 1;

    AdventureSaveProgress(&save);

    // Load into a fresh struct
    AdventureSaveData loaded;
    memset(&loaded, 0, sizeof(loaded));
    AdventureLoadProgress(&loaded);

    ASSERT(loaded.levels[0].unlocked, "level 1 should be unlocked");
    ASSERT(loaded.levels[0].bestStars == 3, "level 1 should have 3 stars");
    ASSERT(loaded.levels[0].completed, "level 1 should be completed");
    ASSERT(loaded.levels[1].unlocked, "level 2 should be unlocked");
    ASSERT(loaded.levels[1].bestStars == 1, "level 2 should have 1 star");

    // Cleanup
    remove("data/adventure_progress.dat");
    PASS();
}

void test_adventure_lives_save_load(void)
{
    TEST("AdventureSaveLives and AdventureLoadLives roundtrip");
    // Remove any existing file first
    remove("data/adventure_lives.dat");

    AdventureSaveData save;
    memset(&save, 0, sizeof(save));

    // Use current time so regen doesn't trigger
    time_t now = time(NULL);
    save.currentLives = 3;
    save.lastLifeRegen = now;

    AdventureSaveLives(&save);

    AdventureSaveData loaded;
    memset(&loaded, 0, sizeof(loaded));
    loaded.currentLives = 0;
    loaded.lastLifeRegen = 0;
    AdventureLoadLives(&loaded);

    ASSERT(loaded.currentLives == 3, "should have 3 lives");
    ASSERT(loaded.lastLifeRegen == now, "should have correct timestamp");

    // Cleanup
    remove("data/adventure_lives.dat");
    PASS();
}

void test_adventure_regen_lives(void)
{
    TEST("AdventureRegenLives regenerates lives over time");
    AdventureSaveData save;
    memset(&save, 0, sizeof(save));

    save.currentLives = 1;
    save.lastLifeRegen = 0; // long ago

    AdventureRegenLives(&save);
    ASSERT(save.currentLives == MAX_LIVES, "should regen to max lives");

    // Test no regen when already at max
    save.currentLives = MAX_LIVES;
    save.lastLifeRegen = 0;
    AdventureRegenLives(&save);
    ASSERT(save.currentLives == MAX_LIVES, "should stay at max");
    PASS();
}

int main(void)
{
    printf("=== Adventure Tests ===\n\n");

    test_adventure_level_defs();
    test_adventure_init_level();
    test_adventure_init_no_obstacles();
    test_adventure_update_progress_clear_lines();
    test_adventure_update_progress_score();
    test_adventure_update_progress_combo();
    test_adventure_check_goal();
    test_adventure_check_failure();
    test_adventure_calc_stars();
    test_adventure_progress_save_load();
    test_adventure_lives_save_load();
    test_adventure_regen_lives();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}