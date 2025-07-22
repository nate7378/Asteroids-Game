#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iostream>

struct Asteroid {
    float x, y;
    float speed;
};

struct Bullet {
    float x, y;
    float speed;
};

std::vector<Asteroid> asteroids;
std::vector<Bullet> bullets;
float spaceshipX = 0.0f, spaceshipY = -0.8f;
int score = 0;
bool gameOver = false;
float baseAsteroidSpeed = 0.01f;

unsigned int spaceshipTexture;
unsigned int asteroidTexture;
unsigned int backgroundTexture;

bool loadTexture(const char* filename, unsigned int& textureID) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        
        stbi_image_free(data);
        return true;
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }
}

void initTextures() {
    if (!loadTexture("ship1.png", spaceshipTexture)) {
        std::cerr << "Failed to load spaceship texture!" << std::endl;
    }
    if (!loadTexture("asteroid.png", asteroidTexture)) {
        std::cerr << "Failed to load asteroid texture!" << std::endl;
    }
    if (!loadTexture("solar.jpg", backgroundTexture)) { // Load the background image
        std::cerr << "Failed to load background texture!" << std::endl;
    }
}

void displayScore() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.9f, 0.9f);

    std::ostringstream scoreStream;
    scoreStream << "Score: " << score;
    std::string scoreString = scoreStream.str();

    for (char c : scoreString) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void displayGameOver() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.2f, 0.0f);

    std::string gameOverText = "Game Over! Press R to Play Again";
    for (char c : gameOverText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void resetGame() {
    asteroids.clear();
    bullets.clear();
    score = 0;
    spaceshipX = 0.0f;
    spaceshipY = -0.8f;
    gameOver = false;
    baseAsteroidSpeed = 0.01f;

    std::cout << "Game reset!" << std::endl;

    for (int i = 0; i < 5; ++i) {
        asteroids.push_back({static_cast<float>(rand() % 200 - 100) / 100.0f, 
                             static_cast<float>(rand() % 200 + 100) / 100.0f, 
                             baseAsteroidSpeed});
    }
}

void drawBackground() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    
    // Adjust based on the aspect ratio of your window
    float aspect = 800.0f / 600.0f; // Assuming window size is 800x600
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-aspect, -1.0f); // Adjusted for aspect ratio
        glTexCoord2f(1.0f, 0.0f); glVertex2f(aspect, -1.0f);  // Adjusted for aspect ratio
        glTexCoord2f(1.0f, 1.0f); glVertex2f(aspect, 1.0f);   // Adjusted for aspect ratio
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-aspect, 1.0f);  // Adjusted for aspect ratio
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

void drawSpaceship() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, spaceshipTexture);

    glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(spaceshipX - 0.1f, spaceshipY - 0.2f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(spaceshipX + 0.1f, spaceshipY - 0.2f);
        glTexCoord2f(0.5f, 1.0f); glVertex2f(spaceshipX, spaceshipY);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
void drawAsteroids() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, asteroidTexture);

    for (const auto& asteroid : asteroids) {
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(asteroid.x - 0.08f, asteroid.y - 0.1f); // Decreased width
            glTexCoord2f(1.0f, 0.0f); glVertex2f(asteroid.x + 0.08f, asteroid.y - 0.1f); // Decreased width
            glTexCoord2f(1.0f, 1.0f); glVertex2f(asteroid.x + 0.08f, asteroid.y + 0.1f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(asteroid.x - 0.08f, asteroid.y + 0.1f);
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawBackground(); // Draw the background first
    drawSpaceship();
    drawAsteroids();

    glColor3f(0.0f, 0.0f, 1.0f);
    for (const auto& bullet : bullets) {
        glBegin(GL_QUADS);
            glVertex2f(bullet.x - 0.02f, bullet.y - 0.02f);
            glVertex2f(bullet.x + 0.02f, bullet.y - 0.02f);
            glVertex2f(bullet.x + 0.02f, bullet.y + 0.02f);
            glVertex2f(bullet.x - 0.02f, bullet.y + 0.02f);
        glEnd();
    }

    displayScore();

    if (gameOver) {
        displayGameOver();
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver) {
        if (score >= 100 && score < 200) {
            baseAsteroidSpeed = 0.02f;
        } else if (score >= 200 && score < 300) {
            baseAsteroidSpeed = 0.03f;
        } else if (score >= 300) {
            baseAsteroidSpeed = 0.04f;
        }

        for (auto& asteroid : asteroids) {
            asteroid.y -= asteroid.speed;
            if (asteroid.y < -1.0f) {
                asteroid.y = static_cast<float>(rand() % 200 + 100) / 100.0f;
                asteroid.x = static_cast<float>(rand() % 200 - 100) / 100.0f;
                asteroid.speed = baseAsteroidSpeed;
            }
        }

        if (asteroids.size() < 5) {
            asteroids.push_back({static_cast<float>(rand() % 200 - 100) / 100.0f,
                                 1.0f, baseAsteroidSpeed});
        }

        for (auto& bullet : bullets) {
            bullet.y += bullet.speed;
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) {
            return b.y > 1.0f;
        }), bullets.end());

        for (size_t i = 0; i < asteroids.size(); ) {
            bool asteroidHit = false;
            for (size_t j = 0; j < bullets.size(); ) {
                if (abs(asteroids[i].x - bullets[j].x) < 0.1f && abs(asteroids[i].y - bullets[j].y) < 0.1f) {
                    asteroids.erase(asteroids.begin() + i);
                    bullets.erase(bullets.begin() + j);
                    asteroidHit = true;
                    score += 10;
                    break;
                } else {
                    ++j;
                }
            }
            if (!asteroidHit) {
                ++i;
            }
        }

        for (const auto& asteroid : asteroids) {
            if (abs(asteroid.x - spaceshipX) < 0.1f && abs(asteroid.y - spaceshipY) < 0.1f) {
                gameOver = true;
                break;
            }
        }

        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
    }
}

void keyboard(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        spaceshipX -= 0.05f;
    } else if (key == GLUT_KEY_RIGHT) {
        spaceshipX += 0.05f;
    }
    spaceshipX = std::max(-1.0f, std::min(spaceshipX, 1.0f)); // Keep spaceship within bounds
}

void keyboardNormal(unsigned char key, int x, int y) {
    if (key == ' ') {
        bullets.push_back({spaceshipX, spaceshipY + 0.1f, 0.05f});
    }
    if ((key == 'r' || key == 'R') && gameOver) {
        std::cout << "Resetting game..." << std::endl;
        resetGame();
        glutPostRedisplay();
        glutTimerFunc(0, update, 0); // Restart the update loop
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initTextures();

    srand(static_cast<unsigned int>(time(nullptr)));
    resetGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Asteroids Game");

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc(keyboardNormal);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
