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
    int x, y, tileSize;
    float moveTime, maxMoveTime;
} Player;
Player player;

int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 3, 1,
    1, 0, 1, 1, 0, 1, 1, 1,
    1, 0, 0, 2, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 1,
    1, 0, 1, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
};

void awake() {
    //APPLICATION
    glfwWindowHint(GLFW_RESIZABLE, 0);

    //GAME
    game.screenWidth = 480;
    game.screenHeight = 480;
    game.currentFrame = 0.f;
    game.lastFrame = 0.f;
    game.deltaTime = 0.f;
    
    //LEVEL
    //level.tileSize = 60;
    level.size = sizeof(map) / sizeof(int);
    level.width = sqrt(level.size);
    level.height = sqrt(level.size);
    
    //PLAYER
    player.x = 6;
    player.y = 6;
    //player.size = 60;
    player.moveTime = 0.f;
    player.maxMoveTime = 1.5f;

    //AUTO TILE SIZE
    int autoSize = game.screenWidth / level.width;
    level.tileSize = autoSize;
    player.tileSize = autoSize;

};

void initialize(GLFWwindow* window) {
    glfwGetFramebufferSize(window, &game.screenWidth, &game.screenHeight);
    glViewport(0, 0, game.screenWidth, game.screenHeight);
    glOrtho(0.0f, game.screenWidth, game.screenHeight, 0.0f, 0.0f, 1.0f);
}

int getTileType(int x, int y) {
    return map[y * level.width + x];
}

void setTileType(int x, int y, int type) {
    map[y * level.width + x] = type;
}

void drawMap() {
    int x, y;
    for (x = 0; x < level.width; x++) {
        for (y = 0; y < level.height; y++) {
            //COLORS
            int tile = getTileType(x, y);
            
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
    glVertex2i(player.x * player.tileSize, player.y * player.tileSize);
    glVertex2i(player.x * player.tileSize, player.y * player.tileSize + player.tileSize);
    glVertex2i(player.x * player.tileSize + player.tileSize, player.y * player.tileSize + player.tileSize);
    glVertex2i(player.x * player.tileSize + player.tileSize, player.y * player.tileSize);
    glEnd();
}

int checkMapHasTileType(int tileType) {
    int i;
    for (i = 0; i < level.size; i++) {
        if (map[i] == tileType) {
            return 1;
        }
    }
    return 0;
}

void moveBox(int currentX, int currentY, int x, int y) {
    if (getTileType(x, y) == 0 || getTileType(x, y) == 3) {
        setTileType(currentX, currentY, 0);
        setTileType(x, y, 2);
    }
}

void checkGameWin() {
    if (!checkMapHasTileType(2)) {
        glfwTerminate();
    }
}

void movePlayer() {
    if (input.up) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x, player.y - 1) == 0 || getTileType(player.x, player.y - 1) == 3) {
                player.y--;
                player.moveTime = 0.f;
            }
            //BOX
            else if (getTileType(player.x, player.y - 1) == 2) {
                moveBox(player.x, player.y - 1, player.x, player.y - 2);
            }

            //CHECK IF AFTER MOVE PLAYER HAS WON
            checkGameWin();
        }
    }
    else if (input.down) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x, player.y + 1) == 0 || getTileType(player.x, player.y + 1) == 3) {
                player.y++;
                player.moveTime = 0.f;
            }
            //BOX
            else if (getTileType(player.x, player.y + 1) == 2) {
                moveBox(player.x, player.y + 1, player.x, player.y + 2);
            }

            //CHECK IF AFTER MOVE PLAYER HAS WON
            checkGameWin();
        }
    }
    else if (input.left) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x - 1, player.y) == 0 || getTileType(player.x - 1, player.y) == 3) {
                player.x--;
                player.moveTime = 0.f;
            }
            //BOX
            else if (getTileType(player.x - 1, player.y) == 2) {
                moveBox(player.x - 1, player.y, player.x - 2, player.y);
            }

            //CHECK IF AFTER MOVE PLAYER HAS WON
            checkGameWin();
        }
    }
    else if (input.right) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x + 1, player.y) == 0 || getTileType(player.x + 1, player.y) == 3) {
                player.x++;
                player.moveTime = 0.f;
            }
            //BOX
            else if (getTileType(player.x + 1, player.y) == 2) {
                moveBox(player.x + 1, player.y, player.x + 2, player.y);
            }

            //CHECK IF AFTER MOVE PLAYER HAS WON
            checkGameWin();
        }
    }
    else {
        player.moveTime = player.maxMoveTime;
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