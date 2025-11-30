#ifndef GRID_H
#define GRID_H

// ============================================================================
// GRID.H - Grid manipulation functions
// ============================================================================

bool isInBounds(int x, int y, int gr_cols, int gr_rows);

bool isTrackTile(char tile);

bool isSwitchTile(char tile);

int getSwitchIndex(char tile);

bool isSpawnPoint(int x, int y, int spawnX[], int spawnY[], int spawnCount);

bool isDestinationPoint(int x, int y, int destX[], int destY[], int destCount);

bool toggleSafetyTile(int x, int y, char grid[][100], int gr_cols, int gr_rows);

#endif

