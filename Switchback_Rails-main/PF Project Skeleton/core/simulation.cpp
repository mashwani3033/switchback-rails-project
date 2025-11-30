#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

// ============================================================================
// SIMULATION.CPP - Implementation of main simulation logic
// ============================================================================

// ----------------------------------------------------------------------------
// INITIALIZE SIMULATION
// ----------------------------------------------------------------------------
void initializeSimulation() {
    // Initialize log files
    initializeLogFiles();
}

// ----------------------------------------------------------------------------
// SIMULATE ONE TICK
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
                    int& trainsDelivered, int& trainsCrashed, int& totalSwitchFlips) {
    
    // Increment tick counter
    currentTick++;
    
    // PHASE 1: SPAWN
    spawnTrainsForTick(currentTick, trainCount, trainSpawnTick, trainX, trainY,
                      trainActive, grid);
    
    // PHASE 2: ROUTE DETERMINATION
    determineAllRoutes(trainCount, trainX, trainY, trainDir,
                      trainNextX, trainNextY, trainNextDir,
                      trainActive, trainCrashed,
                      trainDestX, trainDestY,
                      grid, gridCols, gridRows,
                      switchExists, switchState);
    
    // PHASE 3: COUNTER UPDATE
    updateSwitchCounters(trainCount, trainX, trainY, trainPrevX, trainPrevY,
                        trainActive, trainCrashed, trainDir, grid,
                        switchExists, switchMode, switchCounters);
    
    // PHASE 4: FLIP QUEUE
    queueSwitchFlips(switchExists, switchMode, switchCounters,
                    switchKValues, switchFlipQueued);
    
    // PHASE 5: COLLISION DETECTION
    detectCollisions(trainCount, trainX, trainY, trainNextX, trainNextY, trainNextDir,
                    trainDir, trainDestX, trainDestY,
                    trainActive, trainCrashed, trainDelivered);
    
    // Count ALL crashes (from route determination AND collision detection)
    // Only count and deactivate in ONE place to avoid double-counting
    for(int i = 0; i < trainCount; i++) {
        if(trainCrashed[i] && trainActive[i]) {
            trainsCrashed++;
            trainActive[i] = false;
        }
    }
    
    // PHASE 6: MOVEMENT
    // Save previous positions
    for(int i = 0; i < trainCount; i++) {
        trainPrevX[i] = trainX[i];
        trainPrevY[i] = trainY[i];
    }
    
    moveAllTrains(trainCount, trainX, trainY, trainDir,
                 trainNextX, trainNextY, trainNextDir,
                 trainActive, trainCrashed, trainDelivered,
                 trainWaitTicks, grid);
    
    // PHASE 7: DEFERRED FLIPS
    applyDeferredFlips(switchExists, switchState, switchFlipQueued,
                      switchCounters, totalSwitchFlips);
    
    // PHASE 8: ARRIVALS
    checkArrivals(trainCount, trainX, trainY, trainDestX, trainDestY,
                 trainActive, trainDelivered, trainsDelivered);
    
    // PHASE 9: SIGNAL LIGHTS
    updateSignalLights(switchExists, switchState, switchSignal,
                      grid, gridRows, gridCols,
                      trainCount, trainX, trainY, trainActive);
    
    // PHASE 10: LOGGING
    for(int i = 0; i < trainCount; i++) {
        if(trainActive[i]) {
            const char* state = "MOVING";
            if(trainDelivered[i]) state = "DELIVERED";
            if(trainCrashed[i]) state = "CRASHED";
            
            logTrainTrace(currentTick, i, trainX[i], trainY[i], trainDir[i], state);
        }
    }
    
    for(int i = 0; i < 26; i++) {
        if(switchExists[i]) {
            char letter = 'A' + i;
            const char* mode = switchMode[i] ? "PER_DIR" : "GLOBAL";
            const char* stateName = switchStateNames[i][switchState[i]];
            
            logSwitchState(currentTick, letter, mode, stateName);
        }
    }
    
    for(int i = 0; i < 26; i++) {
        if(switchExists[i]) {
            char letter = 'A' + i;
            const char* signalStr[] = {"GREEN", "YELLOW", "RED"};
            
            logSignalState(currentTick, letter, signalStr[switchSignal[i]]);
        }
    }
    
    // PHASE 11: TERMINAL OUTPUT
    printGridToTerminal(grid, gridRows, gridCols, trainCount,
                       trainX, trainY, trainDir, trainActive, trainCrashed, currentTick);
}

// ----------------------------------------------------------------------------
// CHECK IF SIMULATION IS COMPLETE
// ----------------------------------------------------------------------------
bool isSimulationComplete(int trainCount, bool trainActive[],
                         bool trainDelivered[], bool trainCrashed[]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(trainActive[i] && !trainDelivered[i] && !trainCrashed[i]) {
            return false;
        }
    }
    
    return true;
}

// ----------------------------------------------------------------------------
// PRINT GRID TO TERMINAL (C++ Style)
// ----------------------------------------------------------------------------
void printGridToTerminal(char grid[][100], int gridRows, int gridCols,
                        int trainCount, int trainX[], int trainY[], int trainDir[],
                        bool trainActive[], bool trainCrashed[],
                        int currentTick) {
    
    cout << "\n========== TICK " << currentTick << " ==========\n";
    
    for(int y = 0; y < gridRows; y++) {
        for(int x = 0; x < gridCols; x++) {
            bool trainHere = false;
            int trainId = -1;
            
            for(int i = 0; i < trainCount; i++) {
                if(trainActive[i] && !trainCrashed[i] && 
                   trainX[i] == x && trainY[i] == y) {
                    trainHere = true;
                    trainId = i;
                    break;
                }
            }
            
            if(trainHere) {
                cout << (trainId % 10);
            } else {
                cout << grid[y][x];
            }
        }
        cout << "\n";
    }
    
    cout << "\nTrain Status:\n";
    for(int i = 0; i < trainCount; i++) {
        if(trainActive[i]) {
            const char* dirStr[] = {"UP", "RIGHT", "DOWN", "LEFT"};
            cout << "Train " << i << " at (" << trainX[i] << "," << trainY[i] 
                 << ") moving " << dirStr[trainDir[i]];
            
            if(trainCrashed[i]) cout << " [CRASHED]";
            cout << "\n";
        }
    }
    cout << "==============================\n\n";
}

