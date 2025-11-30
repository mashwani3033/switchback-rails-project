#include "grid.h"
#include "simulation_state.h"

// ============================================================================
// GRID.CPP - Grid utilities
// ============================================================================

// ----------------------------------------------------------------------------
// Check if a position is inside the grid.
// ----------------------------------------------------------------------------
bool isInBounds(int x, int y, int gridCols, int gridRows) {
    return (x >= 0 && x < gridCols && y >= 0 && y < gridRows);
}

// ----------------------------------------------------------------------------
// Check if a tile is a track tile.
// ----------------------------------------------------------------------------
bool isTrackTile(char tile) {
    return (tile == '-' || tile == '|' || tile == '/' || tile == '\\' ||
            tile == '+' || tile == 'S' || tile == 'D' || tile == '=' ||
            (tile >= 'A' && tile <= 'Z'));
}


// ----------------------------------------------------------------------------
// Check if a tile is a switch.
// ----------------------------------------------------------------------------
bool isSwitchTile(char tile) {
    return (tile >= 'A' && tile <= 'Z');
}

// ----------------------------------------------------------------------------
// Get switch index from character.
// ----------------------------------------------------------------------------
int getSwitchIndex(char tile) {
    if(tile >= 'A' && tile <= 'Z') {
        return tile - 'A';  // A=0, B=1, ..., Z=25
    }
    return -1;
}

// ----------------------------------------------------------------------------
// Check if a position is a spawn point.
// ----------------------------------------------------------------------------
bool isSpawnPoint(int x, int y, int spawnX[], int spawnY[], int spawnCount) {
    for(int i = 0; i < spawnCount; i++) {
        if(spawnX[i] == x && spawnY[i] == y) {
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
// Check if a position is a destination.
// ----------------------------------------------------------------------------
bool isDestinationPoint(int x, int y, int destX[], int destY[], int destCount) {
    for(int i = 0; i < destCount; i++) {
        if(destX[i] == x && destY[i] == y) {
            return true;
        }
    } 
    return false;
}

// ----------------------------------------------------------------------------
// Toggle a safety tile.
// ----------------------------------------------------------------------------
bool toggleSafetyTile(int x, int y, char grid[][100], int gridCols, int gridRows) {
    if(!isInBounds(x, y, gridCols, gridRows)) {
        return false;
    }
    
    char tile = grid[y][x];
    
    // If it's a regular track, make it safety
    if(tile == '-' || tile == '|') {
        grid[y][x] = '=';
        return true;
    }
    
    // If it's already safety, revert to track
    if(tile == '=') {
        grid[y][x] = '-';  // Default to horizontal
        return true;
    }
    
    return false;
}

