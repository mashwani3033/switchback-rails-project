#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// ============================================================================
// TRAINS.CPP - Train logic
// ============================================================================

// Storage for planned moves (for collisions).
// Previous positions (to detect switch entry).

// ----------------------------------------------------------------------------
// SPAWN TRAINS FOR CURRENT TICK
// ----------------------------------------------------------------------------
// Activate trains scheduled for this tick.
// ----------------------------------------------------------------------------
void spawnTrainsForTick(int currentTick, int trainCount,
                        int trainSpawnTick[], int trainX[], int trainY[],
                        bool trainActive[], char grid[][100]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(trainSpawnTick[i] == currentTick) {
            trainActive[i] = true;
        }
    }
}

// ----------------------------------------------------------------------------
// DETERMINE NEXT POSITION for a train
// ----------------------------------------------------------------------------
// Compute next position/direction from current tile and rules.
// ----------------------------------------------------------------------------
bool determineNextPosition(int trainId, int trainCount, int trainX[], int trainY[], int trainDir[],
                          int trainNextX[], int trainNextY[], int trainNextDir[],
                          bool trainActive[], bool trainCrashed[],
                          int trainDestX[], int trainDestY[],
                          char grid[][100], int gridCols, int gridRows,
                          bool switchExists[], int switchState[]) {
    
    if(!trainActive[trainId] || trainCrashed[trainId]) {
        return false;
    }
    
    int x = trainX[trainId];
    int y = trainY[trainId];
    int dir = trainDir[trainId];
    
    char currentTile = grid[y][x];
    
    // Determine next direction based on CURRENT tile
    int nextDir = dir;
    
    // At crossing, use smart routing
    if(currentTile == '+') {
        nextDir = getSmartDirectionAtCrossing(x, y, dir, trainDestX[trainId], trainDestY[trainId]);
    } else {
        // Use normal direction logic
        nextDir = getNextDirection(x, y, dir, currentTile, switchExists, switchState);
    }
    
    // Calculate next position based on next direction
    int nextX = x;
    int nextY = y;
    
    if(nextDir == 0) nextY--; // UP
    else if(nextDir == 1) nextX++; // RIGHT
    else if(nextDir == 2) nextY++; // DOWN
    else if(nextDir == 3) nextX--; // LEFT
    
    // If already at destination, don't move (stop here)
    if(x == trainDestX[trainId] && y == trainDestY[trainId]) {
        trainNextX[trainId] = x;
        trainNextY[trainId] = y;
        trainNextDir[trainId] = dir;
        return true;
    }
    
    // Check bounds
    if(nextX < 0 || nextX >= gridCols || nextY < 0 || nextY >= gridRows) {
        trainCrashed[trainId] = true;
        return false;
    }
    
    // Check if next tile is valid track
    char nextTile = grid[nextY][nextX];
    if(!isTrackTile(nextTile)) {
        trainCrashed[trainId] = true;
        return false;
    }
    
    trainNextX[trainId] = nextX;
    trainNextY[trainId] = nextY;
    trainNextDir[trainId] = nextDir;
    
    return true;
}

// ----------------------------------------------------------------------------
// GET NEXT DIRECTION based on current tile and direction
// ----------------------------------------------------------------------------
// Return new direction after entering the tile.
// ----------------------------------------------------------------------------
int getNextDirection(int x, int y, int currentDir, char tile,
                    bool switchExists[], int switchState[]) {
    
    // For straight tracks (-, =, |), continue in current direction
    if(tile == '-' || tile == '=' || tile == '|' || tile == 'S' || tile == 'D') {
        return currentDir;
    }
    
    // For crossings (+), continue straight
    if(tile == '+') {
        return currentDir;
    }
    
    // For switches - always go straight regardless of state
    if(tile >= 'A' && tile <= 'Z') {
        return currentDir;
    }
    
    // For curves
    if(tile == '/') {
        if(currentDir == 0) return 1; // UP -> RIGHT
        if(currentDir == 1) return 0; // RIGHT -> UP
        if(currentDir == 2) return 3; // DOWN -> LEFT
        if(currentDir == 3) return 2; // LEFT -> DOWN
    }
    
    if(tile == '\\') {
        if(currentDir == 0) return 3; // UP -> LEFT
        if(currentDir == 1) return 2; // RIGHT -> DOWN
        if(currentDir == 2) return 1; // DOWN -> RIGHT
        if(currentDir == 3) return 0; // LEFT -> UP
    }
    
    // Default: continue straight
    return currentDir;
}

// ----------------------------------------------------------------------------
// SMART ROUTING AT CROSSING - Route train to its matched destination
// ----------------------------------------------------------------------------
// Choose best direction at '+' toward destination.
// ----------------------------------------------------------------------------
int getSmartDirectionAtCrossing(int x, int y, int currentDir,
                               int destX, int destY) {
    
    int dx = destX - x;
    int dy = destY - y;
    
    // Never go backwards (180 degree turn)
    int backward = (currentDir + 2) % 4;
    
    // Calculate absolute values without abs()
    int absDx = (dx >= 0 ? dx : -dx);
    int absDy = (dy >= 0 ? dy : -dy);
    
    // Prefer the axis with greater distance
    if(absDx > absDy) {
        // Prioritize horizontal movement
        if(dx > 0 && currentDir != backward) return 1; // RIGHT
        if(dx < 0 && currentDir != backward) return 3; // LEFT
    } else if(absDy > absDx) {
        // Prioritize vertical movement
        if(dy > 0 && currentDir != backward) return 2; // DOWN
        if(dy < 0 && currentDir != backward) return 0; // UP
    }
    
    // If equal distance or can't go preferred direction, continue straight
    return currentDir;
}

// ----------------------------------------------------------------------------
// DETERMINE ALL ROUTES (PHASE 2)
// ----------------------------------------------------------------------------
// Fill next positions/directions for all trains.
// ----------------------------------------------------------------------------
void determineAllRoutes(int trainCount, int trainX[], int trainY[], int trainDir[],
                       int trainNextX[], int trainNextY[], int trainNextDir[],
                       bool trainActive[], bool trainCrashed[],
                       int trainDestX[], int trainDestY[],
                       char grid[][100], int gridCols, int gridRows,
                       bool switchExists[], int switchState[]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainCrashed[i]) {
            continue;
        }
        
        int x = trainX[i];
        int y = trainY[i];
        int dir = trainDir[i];
        
        char currentTile = grid[y][x];
        
        // Determine next direction based on CURRENT tile
        int nextDir = dir;
        
        // At crossing, use smart routing
        if(currentTile == '+') {
            nextDir = getSmartDirectionAtCrossing(x, y, dir, trainDestX[i], trainDestY[i]);
        } else {
            // Use normal direction logic
            nextDir = getNextDirection(x, y, dir, currentTile, switchExists, switchState);
        }
        
        // Debug line for train 4
        if(i == 4 && currentTile >= 'A' && currentTile <= 'Z') {
            cout << "Train 4 at switch '" << currentTile << "' (" << x << "," << y 
                 << ") dir=" << dir << ", state=" << switchState[currentTile - 'A'] 
                 << ", nextDir=" << nextDir << "\n";
        }
        
        // Calculate next position based on next direction
        int nextX = x;
        int nextY = y;
        
        if(nextDir == 0) nextY--; // UP
        else if(nextDir == 1) nextX++; // RIGHT
        else if(nextDir == 2) nextY++; // DOWN
        else if(nextDir == 3) nextX--; // LEFT
        
        // If already at destination, don't move (stop here)
        if(x == trainDestX[i] && y == trainDestY[i]) {
            trainNextX[i] = x;
            trainNextY[i] = y;
            trainNextDir[i] = dir;
            continue;
        }

        // Check bounds
        if(nextX < 0 || nextX >= gridCols || nextY < 0 || nextY >= gridRows) {
            trainCrashed[i] = true;
            continue;
        }
        
        // Check if next tile is valid track
        char nextTile = grid[nextY][nextX];
        if(!isTrackTile(nextTile)) {
            trainCrashed[i] = true;
            continue;
        }
        
        trainNextX[i] = nextX;
        trainNextY[i] = nextY;
        trainNextDir[i] = nextDir;
    }
}

// ----------------------------------------------------------------------------
// MOVE ALL TRAINS (PHASE 5)
// ----------------------------------------------------------------------------
// Move trains; resolve collisions and apply effects.
// ----------------------------------------------------------------------------
void moveAllTrains(int trainCount, int trainX[], int trainY[], int trainDir[],
                  int trainNextX[], int trainNextY[], int trainNextDir[],
                  bool trainActive[], bool trainCrashed[], bool trainDelivered[],
                  int trainWaitTicks[], char grid[][100]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainCrashed[i] || trainDelivered[i]) {
            continue;
        }
        
        trainX[i] = trainNextX[i];
        trainY[i] = trainNextY[i];
        trainDir[i] = trainNextDir[i];
    }
}

// ----------------------------------------------------------------------------
// DETECT COLLISIONS WITH PRIORITY SYSTEM
// ----------------------------------------------------------------------------
// Resolve same-tile, swap, and crossing conflicts.
// ----------------------------------------------------------------------------
void detectCollisions(int trainCount, int trainX[], int trainY[],
                     int trainNextX[], int trainNextY[], int trainNextDir[],
                     int trainDir[],
                     int trainDestX[], int trainDestY[],
                     bool trainActive[], bool trainCrashed[], bool trainDelivered[]) {
    
    // Check all pairs of active trains
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainCrashed[i] || trainDelivered[i]) continue;
        
        for(int j = i + 1; j < trainCount; j++) {
            if(!trainActive[j] || trainCrashed[j] || trainDelivered[j]) continue;
            
            // TYPE 1: Same-destination collision (both moving to same next tile)
            if(trainNextX[i] == trainNextX[j] && trainNextY[i] == trainNextY[j]) {
                
                int dist_i = calculateManhattanDistance(trainX[i], trainY[i], 
                                                       trainDestX[i], trainDestY[i]);
                int dist_j = calculateManhattanDistance(trainX[j], trainY[j], 
                                                       trainDestX[j], trainDestY[j]);
                
                if(dist_i == dist_j) {
                    // Equal distance - both crash
                    trainCrashed[i] = true;
                    trainCrashed[j] = true;
                } else if(dist_i > dist_j) {
                    // Train i is FARTHER - it gets priority and MOVES
                    // Train j is CLOSER - it WAITS (stays in current position)
                    trainNextX[j] = trainX[j];
                    trainNextY[j] = trainY[j];
                    trainNextDir[j] = trainDir[j];
                } else {
                    // Train j is FARTHER - it gets priority and MOVES
                    // Train i is CLOSER - it WAITS (stays in current position)
                    trainNextX[i] = trainX[i];
                    trainNextY[i] = trainY[i];
                    trainNextDir[i] = trainDir[i];
                }
            }
            
            // TYPE 2: Head-on swap collision (trains swapping positions)
            if(trainNextX[i] == trainX[j] && trainNextY[i] == trainY[j] &&
               trainNextX[j] == trainX[i] && trainNextY[j] == trainY[i]) {
                
                int dist_i = calculateManhattanDistance(trainX[i], trainY[i], 
                                                       trainDestX[i], trainDestY[i]);
                int dist_j = calculateManhattanDistance(trainX[j], trainY[j], 
                                                       trainDestX[j], trainDestY[j]);
                
                if(dist_i == dist_j) {
                    // Equal distance - both crash
                    trainCrashed[i] = true;
                    trainCrashed[j] = true;
                } else if(dist_i > dist_j) {
                    // Train i moves, train j waits
                    trainNextX[j] = trainX[j];
                    trainNextY[j] = trainY[j];
                    trainNextDir[j] = trainDir[j];
                } else {
                    // Train j moves, train i waits
                    trainNextX[i] = trainX[i];
                    trainNextY[i] = trainY[i];
                    trainNextDir[i] = trainDir[i];
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// CALCULATE MANHATTAN DISTANCE
// ----------------------------------------------------------------------------
int calculateManhattanDistance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
}

// ----------------------------------------------------------------------------
// CHECK ARRIVALS
// ----------------------------------------------------------------------------
// Mark trains that reached destinations.
// ----------------------------------------------------------------------------
void checkArrivals(int trainCount, int trainX[], int trainY[],
                  int trainDestX[], int trainDestY[],
                  bool trainActive[], bool trainDelivered[], int& trainsDelivered) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainDelivered[i]) {
            continue;
        }
        
        // Check if train reached destination (allow exact match)
        if(trainX[i] == trainDestX[i] && trainY[i] == trainDestY[i]) {
            trainDelivered[i] = true;
            trainActive[i] = false;
            trainsDelivered++;
        }
    }
}

// ----------------------------------------------------------------------------
// APPLY EMERGENCY HALT
// ----------------------------------------------------------------------------
// Apply halt to trains in the active zone.
// ----------------------------------------------------------------------------
void applyEmergencyHalt(int trainCount, int trainX[], int trainY[],
                       bool trainActive[], bool trainCrashed[]) {
    // TODO: Implement emergency halt logic when needed
    // This is a stub function for now
}

// ----------------------------------------------------------------------------
// UPDATE EMERGENCY HALT
// ----------------------------------------------------------------------------
// Decrement timer and disable when done.
// ----------------------------------------------------------------------------
void updateEmergencyHalt() {
    // TODO: Implement emergency halt timer logic when needed
    // This is a stub function for now
}

