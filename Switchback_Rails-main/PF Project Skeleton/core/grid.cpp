#include "grid.h"
#include "simulation_state.h"

// ============================================================================
// GRID.CPP - Grid utilities
// ============================================================================

// ----------------------------------------------------------------------------
// Check if position is inside grid
// ----------------------------------------------------------------------------
bool isInBounds(int x, int y, int gr_cols, int gr_rows) {
    return (x >= 0 && x < gr_cols && y >= 0 && y < gr_rows);
}

// ----------------------------------------------------------------------------
// Check if tile is a track tile
// ----------------------------------------------------------------------------
bool isTrackTile(char tile) {
    return (tile == '-' || tile == '|' || tile == '/' || tile == '\\' ||
            tile == '+' || tile == 'S' || tile == 'D' || tile == '=' ||
            (tile >= 'A' && tile <= 'Z'));
}

// ----------------------------------------------------------------------------
// Check if tile is a switch
// ----------------------------------------------------------------------------
bool isSwitchTile(char tile) {
    return (tile >= 'A' && tile <= 'Z');
}

// ----------------------------------------------------------------------------
// Get switch index from character
// ----------------------------------------------------------------------------
int getSwitchIndex(char tile) {
    if(tile >= 'A' && tile <= 'Z') {
        return tile - 'A';
    }
    return -1;
}

// ----------------------------------------------------------------------------
// Check if position is a spawn point
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
// Check if position is a destination
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
// Toggle safety tile at position
// ----------------------------------------------------------------------------
bool toggleSafetyTile(int x, int y, char grid[][100], int gr_cols, int gr_rows) {
    if(!isInBounds(x, y, gr_cols, gr_rows)) {
        return false;
    }
    
    char tile = grid[y][x];
    
    if(tile == '-' || tile == '|') {
        grid[y][x] = '=';
        return true;
    }
    
    if(tile == '=') {
        grid[y][x] = '-';
        return true;
    }
    
    return false;
}

