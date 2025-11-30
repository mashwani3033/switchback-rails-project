#ifndef APP_H
#define APP_H

// ============================================================================
// APP.H - SFML Application
// ============================================================================

bool initializeApp();

void runApp(int& gridRows, int& gridCols, char grid[][100],
           int& trainCount, int trainX[], int trainY[], int trainDir[],
           int trainNextX[], int trainNextY[], int trainNextDir[],
           int trainPrevX[], int trainPrevY[],
           int trainDestX[], int trainDestY[],
           int trainSpawnTick[], int trainColor[],
           bool trainActive[], bool trainCrashed[], bool trainDelivered[],
           int trainWaitTicks[], int trainTotalWaitTicks[],
           bool switchExists[], int switchState[], bool switchMode[],
           int switchCounters[][4], int switchKValues[][4],
           bool switchFlipQueued[], int switchSignal[],
           char switchStateNames[][2][32],
           int& currentTick, int& trainsDelivered, int& trainsCrashed,
           int& totalSwitchFlips);

void cleanupApp();

#endif

