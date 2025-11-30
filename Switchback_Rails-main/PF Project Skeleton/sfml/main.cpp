#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/io.h"
#include "../core/trains.h"
#include <iostream>

using namespace std;

// ============================================================================
// MAIN.CPP - Entry point
// ============================================================================

int main(int argc, char* argv[]) {
    
    int gridRows;
    int gridCols;
    char grid[50][100];
    char levelName[256];
    
    int trainCount;
    int trainX[100];
    int trainY[100];
    int trainDir[100];
    int trainNextX[100];
    int trainNextY[100];
    int trainNextDir[100];
    int trainPrevX[100];
    int trainPrevY[100];
    int trainDestX[100];
    int trainDestY[100];
    int trainSpawnTick[100];
    int trainColor[100];
    bool trainActive[100];
    bool trainCrashed[100];
    bool trainDelivered[100];
    int trainWaitTicks[100];
    int trainTotalWaitTicks[100];
    
    bool switchExists[26];
    int switchState[26];
    bool switchMode[26];
    int switchCounters[26][4];
    int switchKValues[26][4];
    bool switchFlipQueued[26];
    int switchSignal[26];
    char switchStateNames[26][2][32];
    
    int spawnCount;
    int spawnX[50];
    int spawnY[50];
    
    int destCount;
    int destX[50];
    int destY[50];
    
    int currentTick;
    int seed;
    int weatherMode;
    
    int trainsDelivered;
    int trainsCrashed;
    int totalSwitchFlips;
    int signalViolations;
    
    cout << "========================================" << endl;
    cout << "   SWITCHBACK RAILS - Railway Simulator" << endl;
    cout << "========================================" << endl;
    cout << endl;
    
    if(argc < 2) {
        cout << "Usage: " << argv[0] << " <level_file.lvl>" << endl;
        cout << "Example: " << argv[0] << " data/levels/easy_level.lvl" << endl;
        return 1;
    }
    
    const char* levelFile = argv[1];
    
    initializeSimulationState(gridRows, gridCols, grid, levelName,
                              trainCount, trainX, trainY, trainDir,
                              trainNextX, trainNextY, trainNextDir,
                              trainPrevX, trainPrevY,
                              trainDestX, trainDestY, trainSpawnTick, trainColor,
                              trainActive, trainCrashed, trainDelivered,
                              trainWaitTicks, trainTotalWaitTicks,
                              switchExists, switchState, switchMode,
                              switchCounters, switchKValues, switchFlipQueued,
                              switchSignal, switchStateNames,
                              spawnCount, spawnX, spawnY,
                              destCount, destX, destY,
                              currentTick, seed, weatherMode,
                              trainsDelivered, trainsCrashed, totalSwitchFlips, signalViolations);
    
    cout << "Loading level: " << levelFile << endl;
    
    bool loaded = loadLevelFile(levelFile, levelName,
                                gridRows, gridCols, grid,
                                seed, weatherMode,
                                switchExists, switchState, switchMode,
                                switchKValues, switchStateNames,
                                trainCount, trainX, trainY, trainDir,
                                trainDestX, trainDestY, trainSpawnTick, trainColor,
                                spawnCount, spawnX, spawnY,
                                destCount, destX, destY);
    
    if(!loaded) {
        cout << "ERROR: Failed to load level file!" << endl;
        return 1;
    }
    
    cout << "Level loaded: " << levelName << endl;
    cout << "Grid size: " << gridRows << " x " << gridCols << endl;
    cout << "Trains: " << trainCount << endl;
    cout << "Seed: " << seed << endl;
    cout << endl;
    
    initializeSimulation();
    
    bool useSFML = initializeApp();
    
    if(useSFML) {
        cout << "Starting SFML visualization..." << endl;
        
        runApp(gridRows, gridCols, grid,
              trainCount, trainX, trainY, trainDir,
              trainNextX, trainNextY, trainNextDir,
              trainPrevX, trainPrevY,
              trainDestX, trainDestY,
              trainSpawnTick, trainColor,
              trainActive, trainCrashed, trainDelivered,
              trainWaitTicks, trainTotalWaitTicks,
              switchExists, switchState, switchMode,
              switchCounters, switchKValues,
              switchFlipQueued, switchSignal,
              switchStateNames,
              currentTick, trainsDelivered, trainsCrashed,
              totalSwitchFlips);
        
        cleanupApp();
        
    } else {
        cout << "SFML not available. Running console-only simulation..." << endl;
        cout << endl;
        
        const int MAX_TICKS = 500;
        
        spawnTrainsForTick(0, trainCount, trainSpawnTick, trainX, trainY, trainActive, grid);
        
        while(currentTick < MAX_TICKS) {
            simulateOneTick(currentTick,
                           trainCount, trainX, trainY, trainDir,
                           trainNextX, trainNextY, trainNextDir,
                           trainPrevX, trainPrevY,
                           trainDestX, trainDestY,
                           trainSpawnTick, trainColor,
                           trainActive, trainCrashed, trainDelivered,
                           trainWaitTicks, trainTotalWaitTicks,
                           grid, gridRows, gridCols,
                           switchExists, switchState, switchMode,
                           switchCounters, switchKValues,
                           switchFlipQueued, switchSignal,
                           switchStateNames,
                           trainsDelivered, trainsCrashed, totalSwitchFlips);
            
            bool allSpawned = true;
            for(int i = 0; i < trainCount; i++) {
                if(trainSpawnTick[i] > currentTick) {
                    allSpawned = false;
                    break;
                }
            }
            
            if(allSpawned && isSimulationComplete(trainCount, trainActive, trainDelivered, trainCrashed)) {
                cout << "\nSimulation complete at tick " << currentTick << endl;
                break;
            }
        }
    }

    int totalWait = 0;
    for(int i = 0; i < trainCount; i++) {
        totalWait += trainTotalWaitTicks[i];
    }
    
    writeMetrics(currentTick, trainsDelivered, trainsCrashed,
                totalWait, totalSwitchFlips);
    
    cout << endl;
    cout << "========================================" << endl;
    cout << "         SIMULATION COMPLETE" << endl;
    cout << "========================================" << endl;
    cout << "Total Ticks: " << currentTick << endl;
    cout << "Trains Delivered: " << trainsDelivered << endl;
    cout << "Trains Crashed: " << trainsCrashed << endl;
    cout << "Switch Flips: " << totalSwitchFlips << endl;
    cout << endl;
    cout << "Logs saved " << endl;
    
    
    return 0;
}

