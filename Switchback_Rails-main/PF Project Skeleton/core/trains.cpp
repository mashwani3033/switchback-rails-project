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

// ----------------------------------------------------------------------------
// Spawn trains for current tick
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
// Determine next position for a train
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
    
    int nextDir = dir;
    
    if(currentTile == '+') {
        nextDir = getSmartDirectionAtCrossing(x, y, dir, trainDestX[trainId], trainDestY[trainId]);
    } else {
        nextDir = getNextDirection(x, y, dir, currentTile, switchExists, switchState);
    }
    
    int nextX = x;
    int nextY = y;
    
    if(nextDir == 0) 
    nextY--;
    else if(nextDir == 1) 
    nextX++;
    else if(nextDir == 2) 
    nextY++;
    else if(nextDir == 3) 
    nextX--;
    
    if(x == trainDestX[trainId] && y == trainDestY[trainId]) {
        trainNextX[trainId] = x;
        trainNextY[trainId] = y;
        trainNextDir[trainId] = dir;
        return true;
    }
    
    if(nextX < 0 || nextX >= gridCols || nextY < 0 || nextY >= gridRows) {
        trainCrashed[trainId] = true;
        return false;
    }
    
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
// Get next direction based on tile
// ----------------------------------------------------------------------------
int getNextDirection(int x, int y, int currentDir, char tile,
                    bool switchExists[], int switchState[]) {
    
    if(tile == '-' || tile == '=' || tile == '|' || tile == 'S' || tile == 'D') {
        return currentDir;
    }
    
    if(tile == '+') {
        return currentDir;
    }
    
    if(tile >= 'A' && tile <= 'Z') {
        return currentDir;
    }
    
    if(tile == '/') {
        if(currentDir == 0) 
        return 1;
        if(currentDir == 1) 
        return 0;
        if(currentDir == 2) 
        return 3;
        if(currentDir == 3)
         return 2;
    }

    
    if(tile == '\\') {
        if(currentDir == 0) 
        return 3;
        if(currentDir == 1) 
        return 2;
        if(currentDir == 2) 
        return 1;
        if(currentDir == 3)
         return 0;
    }
    
    return currentDir;
}

// ----------------------------------------------------------------------------
// Smart routing at crossing toward destination
// ----------------------------------------------------------------------------
int getSmartDirectionAtCrossing(int x, int y, int currentDir,
                               int destX, int destY) {
    
    int dx = destX - x;
    int dy = destY - y;
    
    int backward = (currentDir + 2) % 4;
    
    int absDx = (dx >= 0 ? dx : -dx);
    int absDy = (dy >= 0 ? dy : -dy);
    
    if(absDx > absDy) {
        if(dx > 0 && currentDir != backward)
         return 1;
        if(dx < 0 && currentDir != backward) 
        return 3;
    } 
    else if(absDy > absDx) {
        if(dy > 0 && currentDir != backward) 
        return 2;
        if(dy < 0 && currentDir != backward) 
        return 0;
    }
    
    return currentDir;
}

// ----------------------------------------------------------------------------
// Determine all routes
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
        
        int nextDir = dir;
        
        if(currentTile == '+') {
            nextDir = getSmartDirectionAtCrossing(x, y, dir, trainDestX[i], trainDestY[i]);
        } else {
            nextDir = getNextDirection(x, y, dir, currentTile, switchExists, switchState);
        }
        
        int nextX = x;
        int nextY = y;
        
        if(nextDir == 0) 
        nextY--;
        else if(nextDir == 1) 
        nextX++;
        else if(nextDir == 2) 
        nextY++;
        else if(nextDir == 3) 
        nextX--;
        
        if(x == trainDestX[i] && y == trainDestY[i]) {
            trainNextX[i] = x;
            trainNextY[i] = y;
            trainNextDir[i] = dir;
            continue;
        }

        if(nextX < 0 || nextX >= gridCols || nextY < 0 || nextY >= gridRows) {
            trainCrashed[i] = true;
            continue;
        }
        
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
// Move all trains
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
// Detect collisions with distance-based priority
// ----------------------------------------------------------------------------
void detectCollisions(int trainCount, int trainX[], int trainY[],
                     int trainNextX[], int trainNextY[], int trainNextDir[],
                     int trainDir[],
                     int trainDestX[], int trainDestY[],
                     bool trainActive[], bool trainCrashed[], bool trainDelivered[]) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainCrashed[i] || trainDelivered[i]) 
        continue;
        
        for(int j = i + 1; j < trainCount; j++) {
            if(!trainActive[j] || trainCrashed[j] || trainDelivered[j]) 
            continue;
            
            if(trainNextX[i] == trainNextX[j] && trainNextY[i] == trainNextY[j]) {
                
                int dist_i = calculateManhattanDistance(trainX[i], trainY[i], 
                                                       trainDestX[i], trainDestY[i]);
                int dist_j = calculateManhattanDistance(trainX[j], trainY[j], 
                                                       trainDestX[j], trainDestY[j]);
                
                if(dist_i == dist_j) {
                    trainCrashed[i] = true;
                    trainCrashed[j] = true;
                } 
                else if(dist_i > dist_j) {
                    trainNextX[j] = trainX[j];
                    trainNextY[j] = trainY[j];
                    trainNextDir[j] = trainDir[j];
                } 
                else {
                    trainNextX[i] = trainX[i];
                    trainNextY[i] = trainY[i];
                    trainNextDir[i] = trainDir[i];
                }
            }
            
            if(trainNextX[i] == trainX[j] && trainNextY[i] == trainY[j] &&
               trainNextX[j] == trainX[i] && trainNextY[j] == trainY[i]) {
                
                int dist_i = calculateManhattanDistance(trainX[i], trainY[i], 
                                                       trainDestX[i], trainDestY[i]);
                int dist_j = calculateManhattanDistance(trainX[j], trainY[j], 
                                                       trainDestX[j], trainDestY[j]);
                
                if(dist_i == dist_j) {
                    trainCrashed[i] = true;
                    trainCrashed[j] = true;
                } 
                else if(dist_i > dist_j) {
                    trainNextX[j] = trainX[j];
                    trainNextY[j] = trainY[j];
                    trainNextDir[j] = trainDir[j];
                } 
                else {
                    trainNextX[i] = trainX[i];
                    trainNextY[i] = trainY[i];
                    trainNextDir[i] = trainDir[i];
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Calculate Manhattan distance
// ----------------------------------------------------------------------------
int calculateManhattanDistance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
}

// ----------------------------------------------------------------------------
// Check arrivals
// ----------------------------------------------------------------------------
void checkArrivals(int trainCount, int trainX[], int trainY[],
                  int trainDestX[], int trainDestY[],
                  bool trainActive[], bool trainDelivered[], int& trainsDelivered) {
    
    for(int i = 0; i < trainCount; i++) {
        if(!trainActive[i] || trainDelivered[i]) {
            continue;
        }
        
        if(trainX[i] == trainDestX[i] && trainY[i] == trainDestY[i]) {
            trainDelivered[i] = true;
            trainActive[i] = false;
            trainsDelivered++;
        }
    }
}

// ----------------------------------------------------------------------------
// Apply emergency halt
// ----------------------------------------------------------------------------
void applyEmergencyHalt(int trainCount, int trainX[], int trainY[],
                       bool trainActive[], bool trainCrashed[]) {
    // Stub function for emergency halt
}

// ----------------------------------------------------------------------------
// Update emergency halt
// ----------------------------------------------------------------------------
void updateEmergencyHalt() {
    // Stub function for emergency halt timer
}

