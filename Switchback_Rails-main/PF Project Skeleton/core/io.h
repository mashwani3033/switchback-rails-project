#ifndef IO_H
#define IO_H

// ============================================================================
// IO.H - Level I/O and logging
// ============================================================================

// ----------------------------------------------------------------------------
// LEVEL LOADING
// ----------------------------------------------------------------------------
bool loadLevelFile(const char* filename,
                   char levelName[],
                   int& gridRows, int& gridCols,
                   char grid[][100],
                   int& seed, int& weatherMode,
                   bool switchExists[], int switchState[], bool switchMode[],
                   int switchKValues[][4], char switchStateNames[][2][32],
                   int& trainCount, int trainX[], int trainY[], int trainDir[],
                   int trainDestX[], int trainDestY[], int trainSpawnTick[], int trainColor[],
                   int& spawnCount, int spawnX[], int spawnY[],
                   int& destCount, int destX[], int destY[]);

// ----------------------------------------------------------------------------
// LOGGING
// ----------------------------------------------------------------------------
void initializeLogFiles();

void logTrainTrace(int tick, int trainId, int x, int y, int dir, const char* state);

void logSwitchState(int tick, char switchLetter, const char* mode, const char* state);

void logSignalState(int tick, char switchLetter, const char* signal);

void writeMetrics(int totalTicks, int trainsDelivered, int trainsCrashed,
                  int totalWaitTicks, int totalSwitchFlips);

#endif

