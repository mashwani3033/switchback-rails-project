#ifndef SIMULATION_H
#define SIMULATION_H

// ============================================================================
// SIMULATION.H - Simulation tick logic
// ============================================================================

// ----------------------------------------------------------------------------
// MAIN SIMULATION FUNCTION
// ----------------------------------------------------------------------------
void simulateOneTick(int& currentTick,
                    int trainCount, int trainX[], int trainY[], int trainDir[],
                    int trainNextX[], int trainNextY[], int trainNextDir[],
                    int trainPrevX[], int trainPrevY[],
                    int trainDestX[], int trainDestY[],
                    int trainSpawnTick[], int trainColor[],
                    bool trainActive[], bool trainCrashed[], bool trainDelivered[],
                    int trainWaitTicks[], int trainTotalWaitTicks[],
                    char grid[][100], int gridRows, int gridCols,
                    bool switchExists[], int switchState[], bool switchMode[],
                    int switchCounters[][4], int switchKValues[][4],
                    bool switchFlipQueued[], int switchSignal[],
                    char switchStateNames[][2][32],
                    int& trainsDelivered, int& trainsCrashed, int& totalSwitchFlips);

// ----------------------------------------------------------------------------
// INITIALIZATION
// ----------------------------------------------------------------------------
void initializeSimulation();

// ----------------------------------------------------------------------------
// UTILITY
// ----------------------------------------------------------------------------
bool isSimulationComplete(int trainCount, bool trainActive[],
                         bool trainDelivered[], bool trainCrashed[]);

// ----------------------------------------------------------------------------
// TERMINAL OUTPUT
// ----------------------------------------------------------------------------
void printGridToTerminal(char grid[][100], int gridRows, int gridCols,
                        int trainCount, int trainX[], int trainY[], int trainDir[],
                        bool trainActive[], bool trainCrashed[],
                        int currentTick);

#endif

