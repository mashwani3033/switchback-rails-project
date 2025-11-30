#include "simulation_state.h"
#include <cstring>

// ============================================================================
// SIMULATION_STATE.CPP - Initialize all state using C++ references
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
) {
    // Reset grid
    gridRows = 0;
    gridCols = 0;
    for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 100; j++) {
            grid[i][j] = ' ';
        }
    }
    levelName[0] = '\0';
    
    // Reset trains
    trainCount = 0;
    for(int i = 0; i < 100; i++) {
        trainX[i] = 0;
        trainY[i] = 0;
        trainDir[i] = 0;
        trainNextX[i] = 0;
        trainNextY[i] = 0;
        trainNextDir[i] = 0;
        trainDestX[i] = 0;
        trainDestY[i] = 0;
        trainSpawnTick[i] = 0;
        trainColor[i] = 0;
        trainActive[i] = false;
        trainCrashed[i] = false;
        trainDelivered[i] = false;
        trainWaitTicks[i] = 0;
        trainTotalWaitTicks[i] = 0;
        trainPrevX[i] = -1;
        trainPrevY[i] = -1;
    }
    
    // Reset switches
    for(int i = 0; i < 26; i++) {
        switchExists[i] = false;
        switchState[i] = 0;
        switchMode[i] = true;
        switchFlipQueued[i] = false;
        switchSignal[i] = 0;
        for(int j = 0; j < 4; j++) {
            switchCounters[i][j] = 0;
            switchKValues[i][j] = 0;
        }
        for(int j = 0; j < 2; j++) {
            switchStateNames[i][j][0] = '\0';
        }
    }
    
    // Reset spawn/dest points
    spawnCount = 0;
    destCount = 0;
    for(int i = 0; i < 50; i++) {
        spawnX[i] = 0;
        spawnY[i] = 0;
        destX[i] = 0;
        destY[i] = 0;
    }
    
    // Reset simulation parameters
    currentTick = 0;
    seed = 0;
    weatherMode = 0;
    
    // Reset metrics
    trainsDelivered = 0;
    trainsCrashed = 0;
    totalSwitchFlips = 0;
    signalViolations = 0;
}

