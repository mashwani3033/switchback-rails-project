#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/grid.h"
#include "../core/switches.h"
#include "../core/io.h"
#include "../core/trains.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>

using namespace std;

// ============================================================================
// APP.CPP - SFML application
// ============================================================================

static sf::RenderWindow* g_window = nullptr;
static sf::Font g_font;
static sf::View g_camera;
static bool g_isPaused = false;
static bool g_isStepMode = false;
static bool g_isDragging = false;
static int g_lastMouseX = 0;
static int g_lastMouseY = 0;
static float g_cellSize = 40.0f;
static float g_gridOffsetX = 50.0f;
static float g_gridOffsetY = 50.0f;

const int TILE_SIZE = 48;

sf::Color trainColors[] = {
    sf::Color::Red, sf::Color::Blue, sf::Color::Green,
    sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan,
    sf::Color::White, sf::Color(255, 165, 0),
    sf::Color(128, 0, 128), sf::Color(255, 192, 203)
};

// ----------------------------------------------------------------------------
// Draw tile
// ----------------------------------------------------------------------------
void drawTile(char tile, int x, int y, int switchState) {
    sf::RectangleShape shape(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
    shape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
    
    if (tile == '-' || tile == '=' || tile == '|') {
        shape.setFillColor(sf::Color(80, 80, 80));
    }
    else if (tile == '/' || tile == '\\') {
        shape.setFillColor(sf::Color(100, 100, 100));
    }
    else if (tile == '+') {
        shape.setFillColor(sf::Color(120, 120, 120));
    }
    else if (tile == 'S') {
        shape.setFillColor(sf::Color(0, 255, 0));
    }
    else if (tile == 'D') {
        shape.setFillColor(sf::Color(255, 100, 0));
    }
    else if (tile >= 'A' && tile <= 'Z') {
        shape.setFillColor(switchState == 0 ? sf::Color(50, 150, 200) : sf::Color(200, 150, 50));
    }
    else {
        shape.setFillColor(sf::Color(30, 30, 30));
    }
    
    g_window->draw(shape);
    
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color(60, 60, 60));
    shape.setOutlineThickness(1);
    g_window->draw(shape);
}

// ----------------------------------------------------------------------------
// Draw train
// ----------------------------------------------------------------------------
void drawTrain(int x, int y, int dir, int colorIndex) {
    sf::CircleShape train(TILE_SIZE / 3);
    train.setPosition(x * TILE_SIZE + TILE_SIZE / 6, y * TILE_SIZE + TILE_SIZE / 6);
    train.setFillColor(trainColors[colorIndex % 10]);
    train.setOutlineColor(sf::Color::White);
    train.setOutlineThickness(2);
    g_window->draw(train);
    
    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    
    float cx = x * TILE_SIZE + TILE_SIZE / 2;
    float cy = y * TILE_SIZE + TILE_SIZE / 2;
    
    if (dir == 0) {
        arrow.setPoint(0, sf::Vector2f(cx, cy - 10));
        arrow.setPoint(1, sf::Vector2f(cx - 5, cy - 5));
        arrow.setPoint(2, sf::Vector2f(cx + 5, cy - 5));
    } else if (dir == 1) {
        arrow.setPoint(0, sf::Vector2f(cx + 10, cy));
        arrow.setPoint(1, sf::Vector2f(cx + 5, cy - 5));
        arrow.setPoint(2, sf::Vector2f(cx + 5, cy + 5));
    } else if (dir == 2) {
        arrow.setPoint(0, sf::Vector2f(cx, cy + 10));
        arrow.setPoint(1, sf::Vector2f(cx - 5, cy + 5));
        arrow.setPoint(2, sf::Vector2f(cx + 5, cy + 5));
    } else {
        arrow.setPoint(0, sf::Vector2f(cx - 10, cy));
        arrow.setPoint(1, sf::Vector2f(cx - 5, cy - 5));
        arrow.setPoint(2, sf::Vector2f(cx - 5, cy + 5));
    }
    
    arrow.setFillColor(sf::Color::White);
    g_window->draw(arrow);
}

// ----------------------------------------------------------------------------
// Draw grid
// ----------------------------------------------------------------------------
void drawGrid(char grid[][100], int gridRows, int gridCols,
             int trainCount, int trainX[], int trainY[], int trainDir[],
             int trainColor[], bool trainActive[], bool trainCrashed[],
             bool switchExists[], int switchState[]) {
    
    for (int y = 0; y < gridRows; y++) {
        for (int x = 0; x < gridCols; x++) {
            char tile = grid[y][x];
            int swState = 0;
            
            if (tile >= 'A' && tile <= 'Z') {
                int idx = tile - 'A';
                if (switchExists[idx]) {
                    swState = switchState[idx];
                }
            }
            
            drawTile(tile, x, y, swState);
        }
    }
    
    for (int i = 0; i < trainCount; i++) {
        if (trainActive[i] && !trainCrashed[i]) {
            drawTrain(trainX[i], trainY[i], trainDir[i], trainColor[i]);
        }
    }
}

// ----------------------------------------------------------------------------
// Draw UI
// ----------------------------------------------------------------------------
void drawUI(int currentTick, int trainsDelivered, int trainsCrashed) {
    sf::RectangleShape panel(sf::Vector2f(g_window->getSize().x, 60));
    panel.setFillColor(sf::Color(0, 0, 0, 180));
    panel.setPosition(0, 0);
    g_window->draw(panel);
    
    sf::CircleShape statusLight(10);
    statusLight.setPosition(20, 25);
    statusLight.setFillColor(g_isPaused ? sf::Color::Red : sf::Color::Green);
    g_window->draw(statusLight);
}

// ----------------------------------------------------------------------------
// Handle input
// ----------------------------------------------------------------------------
void handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            g_isPaused = !g_isPaused;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            if (g_window) g_window->close();
        }
    }
    
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            g_camera.zoom(0.9f);
        } else {
            g_camera.zoom(1.1f);
        }
        g_window->setView(g_camera);
    }
}

// ----------------------------------------------------------------------------
// Initialize application
// ----------------------------------------------------------------------------
bool initializeApp() {
    try {
        g_window = new sf::RenderWindow(sf::VideoMode(1200, 800), "Switchback Rails");
        g_window->setFramerateLimit(60);
        g_camera = g_window->getDefaultView();
        cout << "SFML initialized successfully!" << endl;
        return true;
    } catch (...) {
        cout << "SFML not available." << endl;
        if (g_window) {
            delete g_window;
            g_window = nullptr;
        }
        return false;
    }
}

// ----------------------------------------------------------------------------
// Run application loop
// ----------------------------------------------------------------------------
void runApp(int& gridRows, int& gridCols, char grid[][100],
           int& trainCount, int trainX[], int trainY[], int trainDir[],
           int trainNextX[], int trainNextY[], int trainNextDir[],
           int trainPrevX[], int trainPrevY[],
           int trainDestX[], int trainDestY[],
           int trainSpawnTick[], int trainColor[],
           bool trainActive[], bool trainCrashed[], bool trainDelivered[],
           int trainWaitTicks[], int trainTotalWaitTicks[],
           bool switchExists[], int switchState[], bool switchMode[],
           int switchCounters[][4], int switchKValues[][4],
           bool switchFlipQueued[], int switchSignal[],
           char switchStateNames[][2][32],
           int& currentTick, int& trainsDelivered, int& trainsCrashed,
           int& totalSwitchFlips) {
    
    if (!g_window) return;
    
    float gridPixelWidth = gridCols * TILE_SIZE;
    float gridPixelHeight = gridRows * TILE_SIZE;
    float paddingPixels = 50.0f;
    float desiredViewWidth = gridPixelWidth + (paddingPixels * 2);
    float desiredViewHeight = gridPixelHeight + (paddingPixels * 2);
    
    g_camera.setSize(desiredViewWidth, desiredViewHeight);
    g_camera.setCenter(gridPixelWidth / 2.0f, gridPixelHeight / 2.0f);
    g_window->setView(g_camera);
    
    sf::Clock clock;
    float tickTimer = 0.0f;
    const float TICK_DELAY = 0.5f;
    const int MAX_TICKS = 500;
    
    spawnTrainsForTick(0, trainCount, trainSpawnTick, trainX, trainY, trainActive, grid);
    
    while (g_window->isOpen() && currentTick < MAX_TICKS) {
        sf::Event event;
        while (g_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                g_window->close();
            }
            handleInput(event);
        }
        
        if (!g_isPaused) {
            tickTimer += clock.restart().asSeconds();
            
            if (tickTimer >= TICK_DELAY) {
                tickTimer = 0;
                
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
                for (int i = 0; i < trainCount; i++) {
                    if (trainSpawnTick[i] > currentTick) {
                        allSpawned = false;
                        break;
                    }
                }
                
                if (allSpawned && isSimulationComplete(trainCount, trainActive, trainDelivered, trainCrashed)) {
                    cout << "\nSimulation complete at tick " << currentTick << endl;
                    break;
                }
            }
        } else {
            clock.restart();
        }
        
        g_window->clear(sf::Color(20, 20, 20));
        
        drawGrid(grid, gridRows, gridCols,
                trainCount, trainX, trainY, trainDir,
                trainColor, trainActive, trainCrashed,
                switchExists, switchState);
        
        drawUI(currentTick, trainsDelivered, trainsCrashed);
        
        g_window->display();
    }
}

// ----------------------------------------------------------------------------
// Cleanup application
// ----------------------------------------------------------------------------
void cleanupApp() {
    if (g_window) {
        delete g_window;
        g_window = nullptr;
    }
}