#include "systems/score.h"
#include "core/config.h"
#include <stdio.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) do { printf("  TEST: %s ... ", name); } while(0)
#define PASS() do { printf("PASSED\n"); tests_passed++; } while(0)
#define FAIL(msg) do { printf("FAILED: %s\n", msg); tests_failed++; } while(0)
#define ASSERT(cond, msg) do { if (!(cond)) { FAIL(msg); return; } } while(0)

void test_score_calculate_zero(void)
{
    TEST("ScoreCalculate returns 0 for 0 lines");
    ASSERT(ScoreCalculate(0, 1) == 0, "0 lines should give 0");
    ASSERT(ScoreCalculate(0, 5) == 0, "0 lines with combo should give 0");
    ASSERT(ScoreCalculate(-1, 1) == 0, "negative lines should give 0");
    PASS();
}

void test_score_calculate_basic(void)
{
    TEST("ScoreCalculate basic: lines * combo * BASE_SCORE");
    int s1 = ScoreCalculate(1, 1);
    ASSERT(s1 == BASE_SCORE, "1 line, 1 combo = BASE_SCORE");

    int s2 = ScoreCalculate(2, 1);
    ASSERT(s2 == BASE_SCORE * 2, "2 lines, 1 combo = 2*BASE_SCORE");

    int s3 = ScoreCalculate(3, 2);
    ASSERT(s3 == BASE_SCORE * 3 * 2, "3 lines, 2 combo = 6*BASE_SCORE");
    PASS();
}

void test_score_calculate_combo(void)
{
    TEST("ScoreCalculate with combos");
    int s1 = ScoreCalculate(1, 3);
    ASSERT(s1 == BASE_SCORE * 1 * 3, "1 line, 3 combo = 3*BASE_SCORE");

    int s2 = ScoreCalculate(4, 5);
    ASSERT(s2 == BASE_SCORE * 4 * 5, "4 lines, 5 combo = 20*BASE_SCORE");
    PASS();
}

void test_score_save_load(void)
{
    TEST("ScoreSaveHigh and ScoreLoadHigh roundtrip");
    // Save a known value
    ScoreSaveHigh(12345);
    int loaded = ScoreLoadHigh();
    ASSERT(loaded == 12345, "loaded score should match saved");

    // Save another
    ScoreSaveHigh(99999);
    loaded = ScoreLoadHigh();
    ASSERT(loaded == 99999, "second save should overwrite");

    // Cleanup
    ScoreSaveHigh(0);
    PASS();
}

void test_score_load_default(void)
{
    TEST("ScoreLoadHigh returns 0 when no file exists");
    // Remove file first
    remove("data/highscore.dat");
    int loaded = ScoreLoadHigh();
    ASSERT(loaded == 0, "no file should return 0");
    PASS();
}

int main(void)
{
    printf("=== Score Tests ===\n\n");

    test_score_calculate_zero();
    test_score_calculate_basic();
    test_score_calculate_combo();
    test_score_save_load();
    test_score_load_default();

    printf("\n=== Results: %d passed, %d failed ===\n",
           tests_passed, tests_failed);

    return tests_failed > 0 ? 1 : 0;
}