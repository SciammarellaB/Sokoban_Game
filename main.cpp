#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct game {
    int screenWidth, screenHeight;
    double currentFrame, lastFrame, deltaTime;
} Game;
Game game;

typedef struct {
    int up, down, left, right, action;
} Input;
Input input;

typedef struct {
    int tileSize, size, width, height;
} Level;
Level level;

typedef struct {
    int x, y, size;
} Player;
Player player;

int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 2, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 3, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

void awake() {
    //APPLICATION
    glfwWindowHint(GLFW_RESIZABLE, 0);

    //GAME
    game.screenWidth = 960;
    game.screenHeight = 960;
    
    //LEVEL
    level.tileSize = 120;
    level.size = sizeof(map) / sizeof(int);
    level.width = sqrt(level.size);
    level.height = sqrt(level.size);
    
    //PLAYER
    player.x = 1;
    player.y = 1;
    player.size = 120;
};

void initialize(GLFWwindow* window) {
    glfwGetFramebufferSize(window, &game.screenWidth, &game.screenHeight);
    glViewport(0, 0, game.screenWidth, game.screenHeight);
    glOrtho(0.0f, game.screenWidth, game.screenHeight, 0.0f, 0.0f, 1.0f);
}

void drawMap() {
    int x, y;
    for (x = 0; x < level.width; x++) {
        for (y = 0; y < level.height; y++) {
            //COLORS
            int tile = map[y * level.width + x];
            
            //WALL
            if (tile == 1) {
                glColor3f(1, 1, 1);
            }
            //BOX
            else if (tile == 2) {
                glColor3f(1, 0, 1);
            }
            //DESTINY
            else if (tile == 3) {
                glColor3f(0, 1, 1);
            }
            //VOID
            else {
                glColor3f(0.2, 0.2, 0.2);
            }

            //COUNTERCLOCK
            glBegin(GL_QUADS);
            glVertex2i(x * level.tileSize, y * level.tileSize);
            glVertex2i(x * level.tileSize, y * level.tileSize + level.tileSize);
            glVertex2i(x * level.tileSize + level.tileSize, y * level.tileSize + level.tileSize);
            glVertex2i(x * level.tileSize + level.tileSize, y * level.tileSize);
            glEnd();
        }
    }
}

void drawPlayer() {
    glColor3f(0, 0, 1);
    //COUNTERCLOCK
    glBegin(GL_QUADS);
    glVertex2i(player.x * player.size, player.y * player.size);
    glVertex2i(player.x * player.size, player.y * player.size + player.size);
    glVertex2i(player.x * player.size + player.size, player.y * player.size + player.size);
    glVertex2i(player.x * player.size + player.size, player.y * player.size);
    glEnd();
}

void movePlayer() {
    if (input.up) {
        player.y -= 1;
    }
    if (input.down) {
        player.y += 1;
    }
    if (input.left) {
        player.x -= 1;
    }
    if (input.right) {
        player.x += 1;
    }
}

void update() {
    //MAP
    drawMap();
    //PLAYER
    drawPlayer();
    movePlayer();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_W || key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) input.up = 1;
        if (action == GLFW_RELEASE) input.up = 0;
    }
    if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) input.down = 1;
        if (action == GLFW_RELEASE) input.down = 0;
    }
    if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) input.left = 1;
        if (action == GLFW_RELEASE) input.left = 0;
    }
    if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) input.right = 1;
        if (action == GLFW_RELEASE) input.right = 0;
    }
    if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS) input.action = 1;
        if (action == GLFW_RELEASE) input.action = 0;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //INIT DEFAULT VALUES BEFORE WINDOW
    awake();

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(game.screenWidth, game.screenHeight, "Sokoban", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //INIT DEFAULT VALUES BEFORE WINDOW
    initialize(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        game.currentFrame = glfwGetTime();
        game.deltaTime = game.currentFrame - game.lastFrame;
        game.lastFrame = game.currentFrame;

        //RUN GAME
        update();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        //User Inputs
        glfwSetKeyCallback(window, key_callback);
    }

    glfwTerminate();
    return 0;
}