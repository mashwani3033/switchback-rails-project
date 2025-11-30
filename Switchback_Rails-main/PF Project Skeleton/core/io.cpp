#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

// ============================================================================
// IO.CPP - Level I/O and logging
// ============================================================================

// Helper: Get string length
int getLength(const char* str) {
    int len = 0;
    while(str[len] != '\0') {
        len++;
    }
    return len;
}

// Helper: Find substring in string
const char* findString(const char* text, const char* word) {
    int textLen = getLength(text);
    int wordLen = getLength(word);
    
    for(int i = 0; i <= textLen - wordLen; i++) {
        bool match = true;
        for(int j = 0; j < wordLen; j++) {
            if(text[i + j] != word[j]) {
                match = false;
                break;
            }
        }
        if(match) {
            return &text[i];
        }
    }
    return nullptr;
}

// Helper: Compare first n characters of two strings
int compareFirst(const char* str1, const char* str2, int n) {
    for(int i = 0; i < n; i++) {
        if(str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        if(str1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

// Helper: Compare two full strings
int compareStrings(const char* str1, const char* str2) {
    int i = 0;
    while(str1[i] != '\0' && str2[i] != '\0') {
        if(str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

// Helper: Copy string from source to destination
void copyString(char* dest, const char* src) {
    int i = 0;
    while(src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Helper: Convert string to integer
int toInt(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // Skip whitespace
    while(str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
        i++;
    }
    
    // Handle sign
    if(str[i] == '-') {
        sign = -1;
        i++;
    } else if(str[i] == '+') {
        i++;
    }
    
    // Convert digits
    while(str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return result * sign;
}

// Helper: Read switch data from line
void readSwitchData(const char* line, char& switchLetter, char modeStr[], int& state,
                    int& k0, int& k1, int& k2, int& k3, char name0[], char name1[]) {
    int pos = 0;
    
    // Read switch letter
    switchLetter = line[pos];
    pos++;
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read mode string
    int modeIdx = 0;
    while(line[pos] != ' ' && line[pos] != '\0') {
        modeStr[modeIdx++] = line[pos++];
    }
    modeStr[modeIdx] = '\0';
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read state
    state = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        state = state * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read k0
    k0 = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        k0 = k0 * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read k1
    k1 = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        k1 = k1 * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read k2
    k2 = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        k2 = k2 * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read k3
    k3 = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        k3 = k3 * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read name0
    int name0Idx = 0;
    while(line[pos] != ' ' && line[pos] != '\0') {
        name0[name0Idx++] = line[pos++];
    }
    name0[name0Idx] = '\0';
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read name1
    int name1Idx = 0;
    while(line[pos] != ' ' && line[pos] != '\0' && line[pos] != '\n' && line[pos] != '\r') {
        name1[name1Idx++] = line[pos++];
    }
    name1[name1Idx] = '\0';
}

// Helper: Read train data from line
void readTrainData(const char* line, int& tick, int& x, int& y, int& dir, int& destIdx) {
    int pos = 0;
    
    // Skip leading whitespace
    while(line[pos] == ' ') pos++;
    
    // Read tick
    tick = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        tick = tick * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read x
    x = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        x = x * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read y
    y = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        y = y * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read dir
    dir = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        dir = dir * 10 + (line[pos] - '0');
        pos++;
    }
    
    // Skip space
    while(line[pos] == ' ') pos++;
    
    // Read destIdx
    destIdx = 0;
    while(line[pos] >= '0' && line[pos] <= '9') {
        destIdx = destIdx * 10 + (line[pos] - '0');
        pos++;
    }
}

// ----------------------------------------------------------------------------
// LOAD LEVEL FILE
// ----------------------------------------------------------------------------
// Load a .lvl file into global state.
// ----------------------------------------------------------------------------
bool loadLevelFile(const char* filename,
                   char levelName[],
                   int& gridRows, int& gridCols,
                   char grid[][100],
                   int& seed, int& weatherMode,
                   bool switchExists[], int switchState[], bool switchMode[],
                   int switchKValues[][4], char switchStateNames[][2][32],
                   int& trainCount, int trainX[], int trainY[], int trainDir[],
                   int trainDestX[], int trainDestY[], int trainSpawnTick[], int trainColor[],
                   int& spawnCount, int spawnX[], int spawnY[],
                   int& destCount, int destX[], int destY[]) {
    
    ifstream file(filename);
    if(!file.is_open()) {
        return false;
    }
    
    char line[256];
    
    // Parse NAME
    file.getline(line, 256); // "NAME:"
    file.getline(levelName, 256);
    
    // Skip empty line
    file.getline(line, 256);
    
    // Parse ROWS
    file.getline(line, 256); // "ROWS:"
    file.getline(line, 256);
    gridRows = toInt(line);
    
    // Skip empty line
    file.getline(line, 256);
    
    // Parse COLS
    file.getline(line, 256); // "COLS:"
    file.getline(line, 256);
    gridCols = toInt(line);
    
    // Skip empty line
    file.getline(line, 256);
    
    // Parse SEED
    file.getline(line, 256); // "SEED:"
    file.getline(line, 256);
    seed = toInt(line);
    
    // Skip empty line
    file.getline(line, 256);
    
    // Parse WEATHER
    file.getline(line, 256); // "WEATHER:"
    file.getline(line, 256);
    if(findString(line, "NORMAL")) weatherMode = 0;
    else if(findString(line, "RAIN")) weatherMode = 1;
    else if(findString(line, "FOG")) weatherMode = 2;
    
    // Skip empty line
    file.getline(line, 256);
    
    // Parse MAP
    file.getline(line, 256); // "MAP:"
    
    // Read exactly gridRows lines for the map
    int actualRows = 0;
    for(int row = 0; row < gridRows; row++) {
        if(!file.getline(line, 256)) {
            break; // End of file
        }
        
        // Stop if we hit SWITCHES section
        if(compareFirst(line, "SWITCHES:", 9) == 0) {
            break;
        }
        
        int len = getLength(line);
        for(int x = 0; x < gridCols && x < len; x++) {
            grid[actualRows][x] = line[x];
            
            // Record spawn points
            if(line[x] == 'S') {
                spawnX[spawnCount] = x;
                spawnY[spawnCount] = actualRows;
                spawnCount++;
            }
            
            // Record ALL destination points
            if(line[x] == 'D') {
                destX[destCount] = x;
                destY[destCount] = actualRows;
                destCount++;
            }
        }
        
        // Fill rest with spaces
        for(int x = len; x < gridCols; x++) {
            grid[actualRows][x] = ' ';
        }
        
        actualRows++;
    }
    
    // Fill remaining rows with spaces if needed
    for(int y = actualRows; y < gridRows; y++) {
        for(int x = 0; x < gridCols; x++) {
            grid[y][x] = ' ';
        }
    }
    
    // Parse SWITCHES
    if(compareFirst(line, "SWITCHES:", 9) != 0) {
        file.getline(line, 256); // Read "SWITCHES:"
    }
    
    // Skip empty line if present
    file.getline(line, 256);
    while(getLength(line) > 0 && line[0] >= 'A' && line[0] <= 'Z') {
        char switchLetter;
        char modeStr[32];
        int state, k0, k1, k2, k3;
        char name0[32], name1[32];
        
        readSwitchData(line, switchLetter, modeStr, state, k0, k1, k2, k3, name0, name1);
        
        int idx = switchLetter - 'A';
        switchExists[idx] = true;
        switchState[idx] = state;
        switchMode[idx] = (compareStrings(modeStr, "PER_DIR") == 0);
        switchKValues[idx][0] = k0;
        switchKValues[idx][1] = k1;
        switchKValues[idx][2] = k2;
        switchKValues[idx][3] = k3;
        copyString(switchStateNames[idx][0], name0);
        copyString(switchStateNames[idx][1], name1);
        
        file.getline(line, 256);
    }
    
    // Parse TRAINS
    if(compareFirst(line, "TRAINS:", 7) != 0) {
        file.getline(line, 256); // Read "TRAINS:"
    }
    
    trainCount = 0;
    while(file.getline(line, 256)) {
        if(getLength(line) == 0) continue;
        
        int tick, x, y, dir, destIdx;
        readTrainData(line, tick, x, y, dir, destIdx);
        
        trainSpawnTick[trainCount] = tick;
        trainDir[trainCount] = dir;
        trainColor[trainCount] = destIdx;
        
        // Find nearest spawn point
        int nearestSpawn = 0;
        int minDist = 9999;
        for(int s = 0; s < spawnCount; s++) {
            int dx = spawnX[s] - x;
            int dy = spawnY[s] - y;
            int dist = (dx >= 0 ? dx : -dx) + (dy >= 0 ? dy : -dy);
            if(dist < minDist) {
                minDist = dist;
                nearestSpawn = s;
            }
        }
        
        // Snap train to nearest spawn point
        trainX[trainCount] = spawnX[nearestSpawn];
        trainY[trainCount] = spawnY[nearestSpawn];
        
        // Assign destination
        if(destIdx < destCount) {
            trainDestX[trainCount] = destX[destIdx];
            trainDestY[trainCount] = destY[destIdx];
        }
        
        trainCount++;
    }
    
    file.close();
    return true;
}

// ----------------------------------------------------------------------------
// INITIALIZE LOG FILES
// ----------------------------------------------------------------------------
// Create/clear CSV logs with headers.
// ----------------------------------------------------------------------------
void initializeLogFiles() {
    ofstream traceFile("out/trace.csv");
    if(traceFile.is_open()) {
        traceFile << "Tick,TrainID,X,Y,Direction,State\n";
        traceFile.close();
    }
    
    ofstream switchFile("out/switches.csv");
    if(switchFile.is_open()) {
        switchFile << "Tick,Switch,Mode,State\n";
        switchFile.close();
    }
    
    ofstream signalFile("out/signals.csv");
    if(signalFile.is_open()) {
        signalFile << "Tick,Switch,Signal\n";
        signalFile.close();
    }
}

// ----------------------------------------------------------------------------
// LOG TRAIN TRACE
// ----------------------------------------------------------------------------
// Append tick, train id, position, direction, state to trace.csv.
// ----------------------------------------------------------------------------
void logTrainTrace(int tick, int trainId, int x, int y, int dir, const char* state) {
    ofstream traceFile("out/trace.csv", ios::app);
    if(traceFile.is_open()) {
        const char* dirStr[] = {"UP", "RIGHT", "DOWN", "LEFT"};
        traceFile << tick << "," << trainId << "," << x << "," << y << "," 
                  << dirStr[dir] << "," << state << "\n";
        traceFile.close();
    }
}

// ----------------------------------------------------------------------------
// LOG SWITCH STATE
// ----------------------------------------------------------------------------
// Append tick, switch id/mode/state to switches.csv.
// ----------------------------------------------------------------------------
void logSwitchState(int tick, char switchLetter, const char* mode, const char* state) {
    ofstream switchFile("out/switches.csv", ios::app);
    if(switchFile.is_open()) {
        switchFile << tick << "," << switchLetter << "," << mode << "," << state << "\n";
        switchFile.close();
    }
}

// ----------------------------------------------------------------------------
// LOG SIGNAL STATE
// ----------------------------------------------------------------------------
// Append tick, switch id, signal color to signals.csv.
// ----------------------------------------------------------------------------
void logSignalState(int tick, char switchLetter, const char* signal) {
    ofstream signalFile("out/signals.csv", ios::app);
    if(signalFile.is_open()) {
        signalFile << tick << "," << switchLetter << "," << signal << "\n";
        signalFile.close();
    }
}

// ----------------------------------------------------------------------------
// WRITE FINAL METRICS
// ----------------------------------------------------------------------------
// Write summary metrics to metrics.txt.
// ----------------------------------------------------------------------------
void writeMetrics(int totalTicks, int trainsDelivered, int trainsCrashed,
                 int totalWaitTicks, int totalSwitchFlips) {
    ofstream metricsFile("out/metrics.txt");
    if(metricsFile.is_open()) {
        metricsFile << "=== SWITCHBACK RAILS - SIMULATION METRICS ===\n\n";
        metricsFile << "Total Ticks: " << totalTicks << "\n";
        metricsFile << "Trains Delivered: " << trainsDelivered << "\n";
        metricsFile << "Trains Crashed: " << trainsCrashed << "\n";
        metricsFile << "Total Switch Flips: " << totalSwitchFlips << "\n";
        
        float avgWait = 0.0f;
        if(trainsDelivered > 0) {
            avgWait = (float)totalWaitTicks / trainsDelivered;
        }
        metricsFile << "Average Wait Time: " << avgWait << " ticks\n";
        
        float throughput = 0.0f;
        if(totalTicks > 0) {
            throughput = (float)trainsDelivered * 100.0f / totalTicks;
        }
        metricsFile << "\nThroughput: " << throughput << " trains per 100 ticks\n";
        metricsFile.close();
    }
}

