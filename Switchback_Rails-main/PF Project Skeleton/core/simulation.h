#ifndef SWITCHES_H
#define SWITCHES_H

// ============================================================================
// SWITCHES.H - Switch logic
// ============================================================================

// ----------------------------------------------------------------------------
// SWITCH COUNTER UPDATE
// ----------------------------------------------------------------------------
void updateSwitchCounters(int trainCount, int trainX[], int trainY[],
                         int trainPrevX[], int trainPrevY[],
                         bool trainActive[], bool trainCrashed[],
                         int trainDir[], char grid[][100],
                         bool switchExists[], bool switchMode[],
                         int switchCounters[][4]);

// ----------------------------------------------------------------------------
// FLIP QUEUE
// ----------------------------------------------------------------------------
void queueSwitchFlips(bool switchExists[], bool switchMode[],
                     int switchCounters[][4], int switchKValues[][4],
                     bool switchFlipQueued[]);

// ----------------------------------------------------------------------------
// DEFERRED FLIP
// ----------------------------------------------------------------------------
void applyDeferredFlips(bool switchExists[], int switchState[],
                       bool switchFlipQueued[], int switchCounters[][4],
                       int& totalSwitchFlips);

// ----------------------------------------------------------------------------
// SIGNAL CALCULATION
// ----------------------------------------------------------------------------
void updateSignalLights(bool switchExists[], int switchState[],
                       int switchSignal[], char grid[][100],
                       int gridRows, int gridCols,
                       int trainCount, int trainX[], int trainY[],
                       bool trainActive[]);

// ----------------------------------------------------------------------------
// SWITCH TOGGLE (for manual control)
// ----------------------------------------------------------------------------
void toggleSwitchState(char switchLetter, bool switchExists[], int switchState[]);

// ----------------------------------------------------------------------------
// HELPER FUNCTIONS
// ----------------------------------------------------------------------------
// Get state for a given direction
int getSwitchStateForDirection(int switchIndex, int direction,
                               int switchState[], bool switchMode[]);

#endif

