#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cmath>

// Game constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PLAYER_WIDTH = 30;
const int PLAYER_HEIGHT = 50;
const int PLAYER_DUCK_HEIGHT = 25;
const int OBSTACLE_WIDTH = 30;
const int OBSTACLE_HEIGHT = 55;
const int COLLECTABLE_SIZE = 20;
const int POWERUP_SIZE = 25;
const float INITIAL_GAME_SPEED = 2.0f;
const int GAME_DURATION = 6000; // 60 seconds
const int GROUND_HEIGHT = 50;
const int BOUNDARY_HEIGHT = 30;
const int MAX_HEALTH = 5;
const float JUMP_VELOCITY = 13.0f;
const float GRAVITY = 0.5f;
const int POWERUP_DURATION = 500; // 5 seconds

// Game variables
float playerX = 100;
float playerY = GROUND_HEIGHT;
bool isJumping = false;
bool isDucking = false;
float jumpVelocity = 0;
int score = 0;
int health = MAX_HEALTH;
int gameTime = GAME_DURATION;
float gameSpeed = INITIAL_GAME_SPEED;
bool gameOver = false;
bool coinMagnet = false;
int coinMagnetTime = 0;
bool doublePoints = false;
int doublePointsTime = 0;

// Game objects
struct GameObject {
    float x, y;
    bool active;
    float animationOffset;
    bool isHighObstacle;
};

std::vector<GameObject> obstacles;
std::vector<GameObject> collectables;
std::vector<GameObject> powerups;

// Function prototypes
void display();
void reshape(int w, int h);
void timer(int);
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void drawPlayer();
void drawObstacle(float x, float y, bool isHigh);
void drawCollectable(float x, float y, float offset);
void drawPowerup(float x, float y, float offset, bool isCoinMagnet);
void drawGround();
void drawBoundaries();
void drawHUD();
void updateGame();
void spawnObjects();
void restartGame();
void drawGameOver();
void mouseClick(int button, int state, int x, int y);


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (gameOver) {
        drawGameOver();
    } else {
        drawGround();
        drawBoundaries();
        drawPlayer();

        for (const auto& obj : obstacles) {
            if (obj.active) drawObstacle(obj.x, obj.y, obj.isHighObstacle);
        }

        for (const auto& obj : collectables) {
            if (obj.active) drawCollectable(obj.x, obj.y, obj.animationOffset);
        }

        for (const auto& obj : powerups) {
            if (obj.active) drawPowerup(obj.x, obj.y, obj.animationOffset, obj.isHighObstacle);
        }

        drawHUD();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int) {
    if (!gameOver) {
        updateGame();
    }
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == ' ' && !isJumping && playerY == GROUND_HEIGHT) {
        isJumping = true;
        jumpVelocity = JUMP_VELOCITY;
    }
    if (key == 'd' || key == 'D') {
        isDucking = true;
    }
    if (key == 'r' || key == 'R') {
        if (gameOver) {
            restartGame();
        }
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    if (key == 'd' || key == 'D') {
        isDucking = false;
    }
}

void drawPlayer() {
    glPushMatrix();
    glTranslatef(playerX, playerY, 0);

    float height = isDucking ? PLAYER_DUCK_HEIGHT : PLAYER_HEIGHT;

    // Body (Quad)
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-PLAYER_WIDTH/2, 0);
    glVertex2f(PLAYER_WIDTH/2, 0);
    glVertex2f(PLAYER_WIDTH/2, height);
    glVertex2f(-PLAYER_WIDTH/2, height);
    glEnd();

    // Head (Triangle)
    glColor3f(1.0f, 0.8f, 0.6f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-PLAYER_WIDTH/4, height);
    glVertex2f(PLAYER_WIDTH/4, height);
    glVertex2f(0, height + PLAYER_WIDTH/2);
    glEnd();

    // Eye (Point)
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(PLAYER_WIDTH/8, height + PLAYER_WIDTH/4);
    glEnd();

    // Arm (Line)
    glColor3f(0.0f, 0.0f, 0.8f);
    glBegin(GL_LINES);
    glVertex2f(PLAYER_WIDTH/2, height*3/4);
    glVertex2f(PLAYER_WIDTH, height/2);
    glEnd();

    glPopMatrix();
}

void drawObstacle(float x, float y, bool isHigh) {
    glPushMatrix();
        glTranslatef(x, y, 0);

        float height = isHigh ? OBSTACLE_HEIGHT * 3.0f : 70;
        float width = OBSTACLE_WIDTH;

        // Main body (Rectangle)
        glColor3f(0.0f, 0.5f, 0.0f);  // Dark green
        glBegin(GL_QUADS);
        glVertex2f(-width/4, 0);
        glVertex2f(width/4, 0);
        glVertex2f(width/4, height);
        glVertex2f(-width/4, height);
        glEnd();

        // Left arm (Triangle)
        glBegin(GL_TRIANGLES);
        glVertex2f(-width/4, height * 0.6f);
        glVertex2f(-width/2, height * 0.8f);
        glVertex2f(-width/4, height * 0.9f);
        glEnd();

        // Right arm (Triangle)
        glBegin(GL_TRIANGLES);
        glVertex2f(width/4, height * 0.5f);
        glVertex2f(width/2, height * 0.7f);
        glVertex2f(width/4, height * 0.8f);
        glEnd();

        // Spikes (Lines)
        glColor3f(1.0f, 1.0f, 1.0f);  // White
        glBegin(GL_LINES);
        for (float i = 0.1f; i < 1.0f; i += 0.2f) {
            // Left side spikes
            glVertex2f(-width/4, height * i);
            glVertex2f(-width/3, height * (i + 0.05f));

            // Right side spikes
            glVertex2f(width/4, height * (i + 0.05f));
            glVertex2f(width/3, height * (i + 0.1f));
        }
        glEnd();

        // Top (small circle)
        glColor3f(1.0f, 0.5f, 0.8f);  // Pink
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= 360; i += 10) {
            float radian = i * 3.14159f / 180.0f;
            glVertex2f(cos(radian) * width/8 + 0, sin(radian) * width/8 + height);
        }
        glEnd();

        glPopMatrix();
}

void drawCollectable(float x, float y, float offset) {
    glPushMatrix();
        glTranslatef(x, y + COLLECTABLE_SIZE/2 , 0);

        // Outer circle (Polygon)
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 16; i++) {
            float angle = 2.0f * M_PI * float(i) / 16;
            glVertex2f(cos(angle) * COLLECTABLE_SIZE/2, sin(angle) * COLLECTABLE_SIZE/2);
        }
        glEnd();

        // Inner star (Triangles)
        glColor3f(1.0f, 0.8f, 0.0f);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 5; i++) {
            float angle1 = 2.0f * M_PI * float(i) / 5;
            float angle2 = 2.0f * M_PI * float(i + 1) / 5;
            glVertex2f(0, 0);
            glVertex2f(cos(angle1) * COLLECTABLE_SIZE/3, sin(angle1) * COLLECTABLE_SIZE/3);
            glVertex2f(cos(angle2) * COLLECTABLE_SIZE/3, sin(angle2) * COLLECTABLE_SIZE/3);
        }
        glEnd();

        // Decorative lines (Line Strip)
        glColor3f(1.0f, 0.5f, 0.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 5; i++) {
            float angle = 2.0f * M_PI * float(i) / 5;
            float innerRadius = COLLECTABLE_SIZE/6;
            float outerRadius = COLLECTABLE_SIZE/3;
            glVertex2f(cos(angle) * innerRadius, sin(angle) * innerRadius);
            glVertex2f(cos(angle) * outerRadius, sin(angle) * outerRadius);
        }
        glEnd();

        // Center (Point)
        glColor3f(1.0f, 0.5f, 0.0f);
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        glVertex2f(0, 0);
        glEnd();

        glPopMatrix();
}

const int CIRCLE_SEGMENTS = 20;

void drawPowerup(float x, float y, float offset, bool isCoinMagnet) {
    glPushMatrix();
        glTranslatef(x, y + offset, 0);

        if (isCoinMagnet) {
            // Coin Magnet (Horseshoe Magnet)

            // Horseshoe shape
            glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
                float angle = i * M_PI / CIRCLE_SEGMENTS;
                float c = cos(angle);
                float s = sin(angle);

                // Outer edge (red)
                glColor3f(0.8f - 0.2f * s, 0.2f, 0.2f);
                glVertex2f(c * POWERUP_SIZE, s * POWERUP_SIZE);

                // Inner edge (lighter red)
                glColor3f(1.0f - 0.2f * s, 0.4f, 0.4f);
                glVertex2f(c * POWERUP_SIZE * 0.7f, s * POWERUP_SIZE * 0.7f);
            }
            glEnd();

            // Magnet poles (bright red)
            glColor3f(1.0f, 0.2f, 0.2f);
            glBegin(GL_QUADS);
            // Left pole
            glVertex2f(-POWERUP_SIZE, 0);
            glVertex2f(-POWERUP_SIZE * 0.7f, 0);
            glVertex2f(-POWERUP_SIZE * 0.7f, -POWERUP_SIZE * 0.4f);
            glVertex2f(-POWERUP_SIZE, -POWERUP_SIZE * 0.4f);
            // Right pole
            glVertex2f(POWERUP_SIZE * 0.7f, 0);
            glVertex2f(POWERUP_SIZE, 0);
            glVertex2f(POWERUP_SIZE, -POWERUP_SIZE * 0.4f);
            glVertex2f(POWERUP_SIZE * 0.7f, -POWERUP_SIZE * 0.4f);
            glEnd();

            // Magnetic field lines
            glColor4f(0.9f, 0.4f, 0.4f, 0.7f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_LINES);
            for (int i = 0; i < 5; i++) {
                float y = -POWERUP_SIZE * 0.5f - i * 0.1f * POWERUP_SIZE;
                glVertex2f(-POWERUP_SIZE, y);
                glVertex2f(0, y - POWERUP_SIZE * 0.2f);
                glVertex2f(0, y - POWERUP_SIZE * 0.2f);
                glVertex2f(POWERUP_SIZE, y);
            }
            glEnd();
            glDisable(GL_BLEND);

            // Metallic shine
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i <= CIRCLE_SEGMENTS / 2; i++) {
                float angle = i * M_PI / CIRCLE_SEGMENTS;
                float c = cos(angle);
                float s = sin(angle);
                glVertex2f(c * POWERUP_SIZE * 0.9f, s * POWERUP_SIZE * 0.9f);
                glVertex2f(c * POWERUP_SIZE * 0.8f, s * POWERUP_SIZE * 0.8f);
            }
            glEnd();
            glDisable(GL_BLEND);

        } else {
            // Double Points Powerup (Diamond with 2x)

            // Diamond shape
            glColor3f(0.0f, 0.7f, 1.0f);  // Cyan color
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, POWERUP_SIZE);  // Top
            glVertex2f(POWERUP_SIZE, 0);  // Right
            glVertex2f(0, -POWERUP_SIZE); // Bottom
            glVertex2f(-POWERUP_SIZE, 0); // Left
            glVertex2f(0, POWERUP_SIZE);  // Back to top
            glEnd();

            // Inner diamond (for depth effect)
            glColor3f(0.0f, 0.9f, 1.0f);  // Lighter cyan
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, POWERUP_SIZE * 0.8f);
            glVertex2f(POWERUP_SIZE * 0.8f, 0);
            glVertex2f(0, -POWERUP_SIZE * 0.8f);
            glVertex2f(-POWERUP_SIZE * 0.8f, 0);
            glVertex2f(0, POWERUP_SIZE * 0.8f);
            glEnd();

            // "2x" symbol
            glColor3f(1.0f, 1.0f, 1.0f);  // White color
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            // '2'
            glVertex2f(-POWERUP_SIZE/4, POWERUP_SIZE/4);
            glVertex2f(0, POWERUP_SIZE/4);
            glVertex2f(0, POWERUP_SIZE/4);
            glVertex2f(0, 0);
            glVertex2f(0, 0);
            glVertex2f(-POWERUP_SIZE/4, 0);
            glVertex2f(-POWERUP_SIZE/4, 0);
            glVertex2f(-POWERUP_SIZE/4, -POWERUP_SIZE/4);
            glVertex2f(-POWERUP_SIZE/4, -POWERUP_SIZE/4);
            glVertex2f(0, -POWERUP_SIZE/4);
            // 'x'
            glVertex2f(POWERUP_SIZE/8, POWERUP_SIZE/4);
            glVertex2f(POWERUP_SIZE/3, -POWERUP_SIZE/4);
            glVertex2f(POWERUP_SIZE/8, -POWERUP_SIZE/4);
            glVertex2f(POWERUP_SIZE/3, POWERUP_SIZE/4);
            glEnd();
            glLineWidth(1.0f);

            // Glow effect
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.7f, 1.0f, 0.3f);  // Semi-transparent cyan
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 0);
            for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
                float angle = i * 2.0f * M_PI / CIRCLE_SEGMENTS;
                glVertex2f(cos(angle) * POWERUP_SIZE * 1.5f, sin(angle) * POWERUP_SIZE * 1.5f);
            }
            glEnd();
            glDisable(GL_BLEND);
        }

        glPopMatrix();
}

void drawGround() {
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, GROUND_HEIGHT);
    glVertex2f(0, GROUND_HEIGHT);
    glEnd();

    // Ground details
    glColor3f(0.6f, 0.4f, 0.1f);
    glBegin(GL_LINES);
    for (int i = 0; i < WINDOW_WIDTH; i += 50) {
        glVertex2f(i, GROUND_HEIGHT);
        glVertex2f(i + 25, GROUND_HEIGHT - 10);
    }
    glEnd();
}

void drawBoundaries() {
    // Upper boundary
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= WINDOW_WIDTH; i += 50) {
        glVertex2f(i, WINDOW_HEIGHT);
        glVertex2f(i, WINDOW_HEIGHT - BOUNDARY_HEIGHT);
    }
    glEnd();

    glColor3f(0.7f, 0.7f, 0.7f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < WINDOW_WIDTH; i += 100) {
        glVertex2f(i, WINDOW_HEIGHT - BOUNDARY_HEIGHT);
        glVertex2f(i + 50, WINDOW_HEIGHT - BOUNDARY_HEIGHT);
        glVertex2f(i + 25, WINDOW_HEIGHT - BOUNDARY_HEIGHT - 20);
    }
    glEnd();

    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_POINTS);
    for (int i = 0; i < WINDOW_WIDTH; i += 25) {
        glVertex2f(i, WINDOW_HEIGHT - BOUNDARY_HEIGHT/2);
    }
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);
    for (int i = 0; i < WINDOW_WIDTH; i += 75) {
        glVertex2f(i, WINDOW_HEIGHT);
        glVertex2f(i + 50, WINDOW_HEIGHT - BOUNDARY_HEIGHT);
    }
    glEnd();

    // Lower boundary (just above ground)
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= WINDOW_WIDTH; i += 50) {
        glVertex2f(i, GROUND_HEIGHT);
        glVertex2f(i, GROUND_HEIGHT + BOUNDARY_HEIGHT);
    }
    glEnd();

    glColor3f(0.6f, 0.6f,
 0.6f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < WINDOW_WIDTH; i += 100) {
        glVertex2f(i, GROUND_HEIGHT + BOUNDARY_HEIGHT);
        glVertex2f(i + 50, GROUND_HEIGHT + BOUNDARY_HEIGHT);
        glVertex2f(i + 25, GROUND_HEIGHT + BOUNDARY_HEIGHT + 20);
    }
    glEnd();

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < WINDOW_WIDTH; i += 25) {
        glVertex2f(i, GROUND_HEIGHT + BOUNDARY_HEIGHT/2);
    }
    glEnd();

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    for (int i = 0; i < WINDOW_WIDTH; i += 75) {
        glVertex2f(i, GROUND_HEIGHT);
        glVertex2f(i + 50, GROUND_HEIGHT + BOUNDARY_HEIGHT);
    }
    glEnd();
}

void drawHUD() {
    // Draw health
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < health; i++) {
        glPushMatrix();
        glTranslatef(30 + i * 30, WINDOW_HEIGHT - BOUNDARY_HEIGHT/2, 0);

        // Heart shape (Polygon)
        glBegin(GL_POLYGON);
        for (int j = 0; j < 20; j++) {
            float angle = 2.0f * 3.1415926f * float(j) / 20;
            float x = 16 * pow(sin(angle), 3);
            float y = 13 * cos(angle) - 5 * cos(2*angle) - 2 * cos(3*angle) - cos(4*angle);
            glVertex2f(x, y);
        }
        glEnd();

        // Heart outline (Line Loop)
        glColor3f(0.8f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 20; j++) {
            float angle = 2.0f * 3.1415926f * float(j) / 20;
            float x = 16 * pow(sin(angle), 3);
            float y = 13 * cos(angle) - 5 * cos(2*angle) - 2 * cos(3*angle) - cos(4*angle);
            glVertex2f(x, y);
        }
        glEnd();

        glPopMatrix();
    }

    // Draw score
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(WINDOW_WIDTH - 100, WINDOW_HEIGHT - BOUNDARY_HEIGHT/2);
    std::string scoreStr = "Score: " + std::to_string(score);
    for (char c : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Draw time
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(WINDOW_WIDTH / 2 - 30, WINDOW_HEIGHT - BOUNDARY_HEIGHT/2);
    std::string timeStr = "Time: " + std::to_string(gameTime);
    for (char c : timeStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Draw power-up status
    if (coinMagnet) {
        glColor3f(0.0f, 1.0f, 1.0f);
        glRasterPos2f(10, WINDOW_HEIGHT - BOUNDARY_HEIGHT - 20);
        std::string coinMagnetStr = "Coin Magnet: " + std::to_string(coinMagnetTime);
        for (char c : coinMagnetStr) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }
    if (doublePoints) {
        glColor3f(1.0f, 1.0f, 0.0f);
        glRasterPos2f(10, WINDOW_HEIGHT - BOUNDARY_HEIGHT - 40);
        std::string doublePointsStr = "Double Points: " + std::to_string(doublePointsTime);
        for (char c : doublePointsStr) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }
}

void updateGame() {
    // Update player position
    if (isJumping) {
        playerY += jumpVelocity;
        jumpVelocity -= GRAVITY;
        if (playerY <= GROUND_HEIGHT) {
            playerY = GROUND_HEIGHT;
            isJumping = false;
            jumpVelocity = 0;
        }
    }

    // Move and animate objects
    for (auto& obj : obstacles) {
        if (obj.active) {
            obj.x -= gameSpeed;
            if (obj.x < -OBSTACLE_WIDTH) obj.active = false;
        }
    }

    for (auto& obj : collectables) {
        if (obj.active) {
            obj.x -= gameSpeed;
            obj.animationOffset = sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f) * 5.0f;
            if (obj.x < -COLLECTABLE_SIZE) obj.active = false;

            // Coin magnet effect
            if (coinMagnet && obj.x > playerX) {
                float dx = playerX - obj.x;
                float dy = playerY - obj.y;
                float distance = sqrt(dx * dx + dy * dy);

                // Increase magnet radius from 150 to, for example, 250
                if (distance < 250) {
                    obj.x += dx * 0.1f;
                    obj.y += dy * 0.1f;
                }
            }
        }
    }

    for (auto& obj : powerups) {
        if (obj.active) {
            obj.x -= gameSpeed;
            obj.animationOffset = cos(glutGet(GLUT_ELAPSED_TIME) * 0.005f) * 5.0f;
            if (obj.x < -POWERUP_SIZE) obj.active = false;
        }
    }

    // Spawn new objects
    spawnObjects();

    // Check collisions
    for (auto& obj : obstacles) {
        if (obj.active &&
            playerX < obj.x + OBSTACLE_WIDTH/2 && playerX + PLAYER_WIDTH > obj.x - OBSTACLE_WIDTH/2 &&
            playerY < obj.y + (obj.isHighObstacle ? OBSTACLE_HEIGHT * 1.5 : OBSTACLE_HEIGHT) &&
            playerY + (isDucking ? PLAYER_DUCK_HEIGHT : PLAYER_HEIGHT+10) > obj.y) {
            health--;
            obj.active = false;
            if (health <= 0) {
                gameOver = true;
            }
            playerX = obj.x - PLAYER_WIDTH - 5; // Move player back slightly
            break; // Exit the loop after collision
        }
    }

    for (auto& obj : collectables) {
        if (obj.active &&
            playerX < obj.x + COLLECTABLE_SIZE/2 && playerX + PLAYER_WIDTH > obj.x - COLLECTABLE_SIZE/2 &&
            playerY < obj.y + COLLECTABLE_SIZE && playerY + (isDucking ? PLAYER_DUCK_HEIGHT : PLAYER_HEIGHT) > obj.y) {
            score += (doublePoints ? 2 : 1);
            obj.active = false;
        }
    }

    for (auto& obj : powerups) {
        if (obj.active &&
            playerX < obj.x + POWERUP_SIZE/2 && playerX + PLAYER_WIDTH > obj.x - POWERUP_SIZE/2 &&
            playerY < obj.y + POWERUP_SIZE && playerY + (isDucking ? PLAYER_DUCK_HEIGHT : PLAYER_HEIGHT) > obj.y) {
            if (obj.isHighObstacle) { // Using isHighObstacle to differentiate between powerup types
                coinMagnet = true;
                coinMagnetTime = POWERUP_DURATION;
            } else {
                doublePoints = true;
                doublePointsTime = POWERUP_DURATION;
            }
            obj.active = false;
        }
    }

    // Update power-up timers
    if (coinMagnet) {
        coinMagnetTime--;
        if (coinMagnetTime <= 0) {
            coinMagnet = false;
        }
    }
    if (doublePoints) {
        doublePointsTime--;
        if (doublePointsTime <= 0) {
            doublePoints = false;
        }
    }

    // Update game state
    gameTime--;
    if (gameTime <= 0) {
        gameOver = true;
    }

    // Increase game speed over time
    gameSpeed += 0.001f;
}

void spawnObjects() {
    if (rand() % 800 < 2) {  // Now approximately 0.5% chance to spawn an obstacle per frame
        bool isHigh = rand() % 2 == 0;
        obstacles.push_back({WINDOW_WIDTH, float(GROUND_HEIGHT + (isHigh ? OBSTACLE_HEIGHT : 0)), true, 0, isHigh});
    }
    if (rand() % 200 < 3) {
        collectables.push_back({WINDOW_WIDTH, float(GROUND_HEIGHT + rand() % 100), true, 0, false});
    }

    if (rand() % 1200 < 5) {
        bool isCoinMagnet = rand() % 2 == 0;
        powerups.push_back({WINDOW_WIDTH, float(GROUND_HEIGHT + rand() % 100), true, 0, isCoinMagnet});
    }
}

void restartGame() {
    playerX = 100;
    playerY = GROUND_HEIGHT;
    isJumping = false;
    isDucking = false;
    jumpVelocity = 0;
    score = 0;
    health = MAX_HEALTH;
    gameTime = GAME_DURATION;
    gameSpeed = INITIAL_GAME_SPEED;
    gameOver = false;
    coinMagnet = false;
    coinMagnetTime = 0;
    doublePoints = false;
    doublePointsTime = 0;
    obstacles.clear();
    collectables.clear();
    powerups.clear();
}

void drawGameOver() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
    std::string gameOverStr;
    if (gameTime <= 0) {
        gameOverStr = "GAME END";
    } else if (health <= 0) {
        gameOverStr = "GAME LOST";
    } else {
        gameOverStr = "GAME OVER";  // Fallback, shouldn't normally occur
    }
    for (char c : gameOverStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glRasterPos2f(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 - 30);
    std::string scoreStr = "Final Score: " + std::to_string(score);
    for (char c : scoreStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Draw restart button
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 - 80);
    glVertex2f(WINDOW_WIDTH / 2 + 60, WINDOW_HEIGHT / 2 - 80);
    glVertex2f(WINDOW_WIDTH / 2 + 60, WINDOW_HEIGHT / 2 - 50);
    glVertex2f(WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2 - 50);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(WINDOW_WIDTH / 2 - 30, WINDOW_HEIGHT / 2 - 70);
    std::string restartStr = "Restart";
    for (char c : restartStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (gameOver && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        y = windowHeight - y; // Invert y coordinate

        if (x >= WINDOW_WIDTH / 2 - 60 && x <= WINDOW_WIDTH / 2 + 60 &&
            y >= WINDOW_HEIGHT / 2 - 80 && y <= WINDOW_HEIGHT / 2 - 50) {
            restartGame();
        }
    }
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("2D Infinite Runner");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutMouseFunc(mouseClick);

    srand(time(0));

    glutMainLoop();
    return 0;
}
