#include "switches.h"
#include "simulation_state.h"
#include "grid.h"
#include "io.h"

using namespace std;

// ============================================================================
// SWITCHES.CPP - Switch management
// ============================================================================

// ----------------------------------------------------------------------------
// UPDATE SWITCH COUNTERS
// ----------------------------------------------------------------------------
// Increment counters for trains entering switches.
// ----------------------------------------------------------------------------
void updateSwitchCounters(int trainCount, int trainX[], int trainY[],
                         int trainPrevX[], int trainPrevY[],
                         bool trainActive[], bool trainCrashed[],
                         int trainDir[], char grid[][100],
                         bool switchExists[], bool switchMode[],
                         int switchCounters[][4]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainCrashed[i]) continue;
        
        // Check if train just entered a switch tile
        int x = trainX[i];
        int y = trainY[i];
        int px = trainPrevX[i];
        int py = trainPrevY[i];
        
        // If previous position was different and current tile is a switch
        if((px != x || py != y) && isSwitchTile(grid[y][x])) {
            int idx = getSwitchIndex(grid[y][x]);
            
            if(switchExists[idx]) {
                int dir = trainDir[i];
                
                if(switchMode[idx]) {
                    // PER_DIR mode: increment specific direction counter
                    switchCounters[idx][dir]++;
                } else {
                    // GLOBAL mode: increment only counter[0] (shared counter)
                    switchCounters[idx][0]++;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// QUEUE SWITCH FLIPS
// ----------------------------------------------------------------------------
// Queue flips when counters hit K.
// ----------------------------------------------------------------------------
void queueSwitchFlips(bool switchExists[], bool switchMode[],
                     int switchCounters[][4], int switchKValues[][4],
                     bool switchFlipQueued[]) {
    
    for(int i = 0; i < 26; i++) {
        if(!switchExists[i]) continue;
        
        bool shouldFlip = false;
        
        if(switchMode[i]) {
            // PER_DIR: check each direction
            for(int dir = 0; dir < 4; dir++) {
                if(switchCounters[i][dir] >= switchKValues[i][dir] && 
                   switchKValues[i][dir] > 0) {
                    shouldFlip = true;
                    break;
                }
            }
        } else {
            // GLOBAL: check first counter (all are same)
            if(switchCounters[i][0] >= switchKValues[i][0] && 
               switchKValues[i][0] > 0) {
                shouldFlip = true;
            }
        }
        
        if(shouldFlip) {
            switchFlipQueued[i] = true;
        }
    }
}

// ----------------------------------------------------------------------------
// APPLY DEFERRED FLIPS
// ----------------------------------------------------------------------------
// Apply queued flips after movement.
// ----------------------------------------------------------------------------
void applyDeferredFlips(bool switchExists[], int switchState[],
                       bool switchFlipQueued[], int switchCounters[][4],
                       int& totalSwitchFlips) {
    
    for(int i = 0; i < 26; i++) {
        if(!switchExists[i]) continue;
        
        if(switchFlipQueued[i]) {
            // Flip the switch state
            switchState[i] = 1 - switchState[i]; // 0->1 or 1->0
            
            // Reset counters
            for(int dir = 0; dir < 4; dir++) {
                switchCounters[i][dir] = 0;
            }
            
            // Clear flip queue
            switchFlipQueued[i] = false;
            
            // Increment metric
            totalSwitchFlips++;
        }
    }
}

// ----------------------------------------------------------------------------
// UPDATE SIGNAL LIGHTS
// ----------------------------------------------------------------------------
// Update signal colors for switches.
// ----------------------------------------------------------------------------
void updateSignalLights(bool switchExists[], int switchState[],
                       int switchSignal[], char grid[][100],
                       int gridRows, int gridCols,
                       int trainCount, int trainX[], int trainY[],
                       bool trainActive[]) {
    
    // For each switch, check if next tile is clear
    for(int i = 0; i < 26; i++) {
        if(!switchExists[i]) continue;
        
        // Find a switch tile on the grid
        bool found = false;
        int sx = -1, sy = -1;
        
        for(int y = 0; y < gridRows && !found; y++) {
            for(int x = 0; x < gridCols && !found; x++) {
                if(grid[y][x] == ('A' + i)) {
                    sx = x;
                    sy = y;
                    found = true;
                }
            }
        }
        
        if(!found) continue;
        
        // Check if any train is near this switch
        int minDist = 999;
        for(int t = 0; t < trainCount; t++) {
            if(!trainActive[t]) continue;
            
            // Calculate Manhattan distance without abs()
            int dx = trainX[t] - sx;
            int dy = trainY[t] - sy;
            int dist = (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
            
            if(dist < minDist) {
                minDist = dist;
            }
        }
        
        // Set signal based on distance
        if(minDist == 0) {
            switchSignal[i] = 2; // RED: occupied
        } else if(minDist <= 2) {
            switchSignal[i] = 1; // YELLOW: nearby
        } else {
            switchSignal[i] = 0; // GREEN: clear
        }
    }
}

// ----------------------------------------------------------------------------
// TOGGLE SWITCH STATE (Manual)
// ----------------------------------------------------------------------------
// Manually toggle a switch state.
// ----------------------------------------------------------------------------
void toggleSwitchState(char switchLetter, bool switchExists[], int switchState[]) {
    if(switchLetter >= 'A' && switchLetter <= 'Z') {
        int idx = switchLetter - 'A';
        if(switchExists[idx]) {
            switchState[idx] = 1 - switchState[idx];
        }
    }
}

// ----------------------------------------------------------------------------
// GET SWITCH STATE FOR DIRECTION
// ----------------------------------------------------------------------------
// Return the state for a given direction.
// ----------------------------------------------------------------------------
int getSwitchStateForDirection(int switchIndex, int direction,
                               int switchState[], bool switchMode[]) {
    // For now, just return the current switch state
    // This can be enhanced later if direction-specific logic is needed
    if(switchIndex >= 0 && switchIndex < 26) {
        return switchState[switchIndex];
    }
    return 0;
}

