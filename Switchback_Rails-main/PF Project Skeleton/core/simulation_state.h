#ifndef SIMULATION_STATE_H
#define SIMULATION_STATE_H

// ============================================================================
// SIMULATION_STATE.H - Initialization only (NO EXTERN!)
// ============================================================================

void initializeSimulationState(
    int& gridRows, int& gridCols, char grid[][100], char levelName[],
    int& trainCount, int trainX[], int trainY[], int trainDir[],
    int trainNextX[], int trainNextY[], int trainNextDir[],
    int trainPrevX[], int trainPrevY[],
    int trainDestX[], int trainDestY[], int trainSpawnTick[], int trainColor[],
    bool trainActive[], bool trainCrashed[], bool trainDelivered[],
    int trainWaitTicks[], int trainTotalWaitTicks[],
    bool switchExists[], int switchState[], bool switchMode[],
    int switchCounters[][4], int switchKValues[][4], bool switchFlipQueued[],
    int switchSignal[], char switchStateNames[][2][32],
    int& spawnCount, int spawnX[], int spawnY[],
    int& destCount, int destX[], int destY[],
    int& currentTick, int& seed, int& weatherMode,
    int& trainsDelivered, int& trainsCrashed, int& totalSwitchFlips, int& signalViolations
);

#endif

