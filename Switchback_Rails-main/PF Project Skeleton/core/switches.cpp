#include "switches.h"
#include "simulation_state.h"
#include "grid.h"
#include "io.h"

using namespace std;

// ============================================================================
// SWITCHES.CPP - Switch management
// ============================================================================

// ----------------------------------------------------------------------------
// Update switch counters
// ----------------------------------------------------------------------------
void updateSwitchCounters(int trainCount, int trainX[], int trainY[],
                         int trainPrevX[], int trainPrevY[],
                         bool trainActive[], bool trainCrashed[],
                         int trainDir[], char grid[][100],
                         bool switchExists[], bool switchMode[],
                         int switchCounters[][4]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainCrashed[i]) continue;
        
        int x = trainX[i];
        int y = trainY[i];
        int px = trainPrevX[i];
        int py = trainPrevY[i];
        
        if((px != x || py != y) && isSwitchTile(grid[y][x])) {
            int idx = getSwitchIndex(grid[y][x]);
            
            if(switchExists[idx]) {
                int dir = trainDir[i];
                
                if(switchMode[idx]) {
                    switchCounters[idx][dir]++;
                } else {
                    switchCounters[idx][0]++;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Queue switch flips
// ----------------------------------------------------------------------------
void queueSwitchFlips(bool switchExists[], bool switchMode[],
                     int switchCounters[][4], int switchKValues[][4],
                     bool switchFlipQueued[]) {
    
    for(int i = 0; i < 26; i++) {
        if(!switchExists[i]) continue;
        
        bool shouldFlip = false;
        
        if(switchMode[i]) {
            for(int dir = 0; dir < 4; dir++) {
                if(switchCounters[i][dir] >= switchKValues[i][dir] && 
                   switchKValues[i][dir] > 0) {
                    shouldFlip = true;
                    break;
                }
            }
        } else {
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
// Apply deferred flips
// ----------------------------------------------------------------------------
void applyDeferredFlips(bool switchExists[], int switchState[],
                       bool switchFlipQueued[], int switchCounters[][4],
                       int& totalSwitchFlips) {
    
    for(int i = 0; i < 26; i++) {
        if(!switchExists[i]) {
            continue;
        }
        
        if(switchFlipQueued[i]) {
        
            switchState[i] = 1 - switchState[i]; 
            
            for(int dir = 0; dir < 4; dir++) {
                switchCounters[i][dir] = 0;
            }
            
            switchFlipQueued[i] = false;
            
            totalSwitchFlips++;
        }
    }
}

// ----------------------------------------------------------------------------
// Update signal lights
// ----------------------------------------------------------------------------
void updateSignalLights(bool switchExists[], int switchState[],
                       int switchSignal[], char grid[][100],
                       int gridRows, int gridCols,
                       int trainCount, int trainX[], int trainY[],
                       bool trainActive[]) {

    for(int i = 0; i < 26; i++) {
        if(!switchExists[i]) continue;

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
        
        int minDist = 999;
        for(int t = 0; t < trainCount; t++) {
            if(!trainActive[t]) {
                continue;
            }
            
            int dx = trainX[t] - sx;
            int dy = trainY[t] - sy;
            int dist = (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
            
            if(dist < minDist) {
                minDist = dist;
            }
        }
        
        if(minDist == 0) {
            switchSignal[i] = 2;
        } else if(minDist <= 2) {
            switchSignal[i] = 1;
        } else {
            switchSignal[i] = 0;
        }
    }
}

// ----------------------------------------------------------------------------
// Toggle switch state
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
// Get switch state for direction
// ----------------------------------------------------------------------------
int getSwitchStateForDirection(int switchIndex, int direction,
                               int switchState[], bool switchMode[]) {
                               
    if((switchIndex >= 0) && (switchIndex < 26)) {
        return switchState[switchIndex];
    }
    return 0;
}

