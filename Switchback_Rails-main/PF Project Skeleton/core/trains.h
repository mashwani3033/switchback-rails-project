#ifndef TRAINS_H
#define TRAINS_H

// ============================================================================
// TRAINS.H - Train logic
// ============================================================================

// ----------------------------------------------------------------------------
// TRAIN SPAWNING
// ----------------------------------------------------------------------------
void spawnTrainsForTick(int currentTick, int trainCount,
                        int trainSpawnTick[], int trainX[], int trainY[],
                        bool trainActive[], char grid[][100]);

// ----------------------------------------------------------------------------
// TRAIN ROUTING
// ----------------------------------------------------------------------------
void determineAllRoutes(int trainCount, int trainX[], int trainY[], int trainDir[],
                       int trainNextX[], int trainNextY[], int trainNextDir[],
                       bool trainActive[], bool trainCrashed[],
                       int trainDestX[], int trainDestY[],
                       char grid[][100], int gridCols, int gridRows,
                       bool switchExists[], int switchState[]);

bool determineNextPosition(int trainId, int trainCount, int trainX[], int trainY[], int trainDir[],
                          int trainNextX[], int trainNextY[], int trainNextDir[],
                          bool trainActive[], bool trainCrashed[],
                          int trainDestX[], int trainDestY[],
                          char grid[][100], int gridCols, int gridRows,
                          bool switchExists[], int switchState[]);

int getNextDirection(int x, int y, int currentDir, char tile,
                    bool switchExists[], int switchState[]);

int getSmartDirectionAtCrossing(int x, int y, int currentDir,
                               int destX, int destY);

// ----------------------------------------------------------------------------
// TRAIN MOVEMENT
// ----------------------------------------------------------------------------
void moveAllTrains(int trainCount, int trainX[], int trainY[], int trainDir[],
                  int trainNextX[], int trainNextY[], int trainNextDir[],
                  bool trainActive[], bool trainCrashed[], bool trainDelivered[],
                  int trainWaitTicks[], char grid[][100]);

// ----------------------------------------------------------------------------
// COLLISION DETECTION
// ----------------------------------------------------------------------------
void detectCollisions(int trainCount, int trainX[], int trainY[],
                     int trainNextX[], int trainNextY[], int trainNextDir[],
                     int trainDir[],
                     int trainDestX[], int trainDestY[],
                     bool trainActive[], bool trainCrashed[], bool trainDelivered[]);

int calculateManhattanDistance(int x1, int y1, int x2, int y2);

// ----------------------------------------------------------------------------
// ARRIVALS
// ----------------------------------------------------------------------------
void checkArrivals(int trainCount, int trainX[], int trainY[],
                  int trainDestX[], int trainDestY[],
                  bool trainActive[], bool trainDelivered[],
                  int& trainsDelivered);

// ----------------------------------------------------------------------------
// EMERGENCY HALT
// ----------------------------------------------------------------------------
void applyEmergencyHalt(int trainCount, int trainX[], int trainY[],
                       bool trainActive[], bool trainCrashed[]);

void updateEmergencyHalt();

#endif

