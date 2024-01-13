#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct {
    int value1, value2;
} Tuple;

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

typedef struct {
    int x, y, tileSize;
} Box;
Box box;

int map[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 2, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

void closeApplication() {
    glfwTerminate();
    exit(-1);
}

void loadTexture() {
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char* img_data = stbi_load("MapTexture.png", &width, &height, &nrChannels, 0);

    if (!img_data) {
        printf("ERROR LOADING IMAGE! \n");
        closeApplication();
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    if(img_data)
        stbi_image_free(img_data);
}

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

    //BOX
    box.x = 3;
    box.y = 3;

    //AUTO TILE SIZE
    int autoSize = game.screenWidth / level.width;
    level.tileSize = autoSize;
    player.tileSize = autoSize;
    box.tileSize = autoSize;

};

void initialize(GLFWwindow* window) {
    glfwGetFramebufferSize(window, &game.screenWidth, &game.screenHeight);
    glViewport(0, 0, game.screenWidth, game.screenHeight);
    glOrtho(0.0f, game.screenWidth, game.screenHeight, 0.0f, 0.0f, 1.0f);
    loadTexture();
}

int getTileType(int x, int y) {
    return map[y * level.width + x];
}

void setTileType(int x, int y, int type) {
    map[y * level.width + x] = type;
}

Tuple getFirstTileCoordinatesByType(int type) {
    Tuple tuple;
    tuple.value1 = -1;
    tuple.value2 = -1;
    int x, y;
    for (x = 0; x < level.width; x++) {
        for (y = 0; y < level.height; y++) {
            if (getTileType(x, y) == type) {
                tuple.value1 = x;
                tuple.value2 = y;
                return tuple;
            }
        }
    }
    return tuple;
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
            //DESTINY
            else if (tile == 2) {
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

void drawBox() {
    glColor3f(1, 1, 0);
    //COUNTERCLOCK
    glBegin(GL_QUADS);
    glVertex2i(box.x * box.tileSize, box.y * box.tileSize);
    glVertex2i(box.x * box.tileSize, box.y * box.tileSize + box.tileSize);
    glVertex2i(box.x * box.tileSize + box.tileSize, box.y * box.tileSize + box.tileSize);
    glVertex2i(box.x * box.tileSize + box.tileSize, box.y * box.tileSize);
    glEnd();
}

//RECEIVE DE BOUNDING OF OBJECT AND CHECK IF IT WILL INTERSECT WITH IT
int boxAABB(int x, int y) {
    if(box.x == x && box.y == y)
        return 1;
    return 0;
}

void moveBox(int x, int y) {
    int xo = box.x + x;
    int yo = box.y + y;
    if (getTileType(xo, yo) == 0 || getTileType(xo, yo) == 2) {
        box.x = xo;
        box.y = yo;
    }
}

void checkGameWin() {
    Tuple tuple = getFirstTileCoordinatesByType(2);
    if (tuple.value1 == box.x && tuple.value2 == box.y){
        closeApplication();
    }
}

void movePlayer() {
    if (input.up) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x, player.y - 1) == 0 || getTileType(player.x, player.y - 1) == 2) {
                if (boxAABB(player.x, player.y - 1)) {
                    moveBox(0, -1);

                    //CHECK IF AFTER MOVE PLAYER HAS WON
                    checkGameWin();
                }
                else {
                    player.y--;
                    player.moveTime = 0.f;
                }
            }
        }
    }
    else if (input.down) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x, player.y + 1) == 0 || getTileType(player.x, player.y + 1) == 2) {
                if (boxAABB(player.x, player.y + 1)) {
                    moveBox(0, 1);

                    //CHECK IF AFTER MOVE PLAYER HAS WON
                    checkGameWin();
                }
                else {
                    player.y++;
                    player.moveTime = 0.f;
                }
            }
        }
    }
    else if (input.left) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x - 1, player.y) == 0 || getTileType(player.x - 1, player.y) == 2) {
                if (boxAABB(player.x - 1, player.y)) {
                    moveBox(-1, 0);

                    //CHECK IF AFTER MOVE PLAYER HAS WON
                    checkGameWin();
                }
                else {
                    player.x--;
                    player.moveTime = 0.f;
                }
            }
        }
    }
    else if (input.right) {
        player.moveTime += 7 * game.deltaTime;
        if (player.moveTime >= player.maxMoveTime) {
            //VOID DESTINY
            if (getTileType(player.x + 1, player.y) == 0 || getTileType(player.x + 1, player.y) == 2) {
                if (boxAABB(player.x + 1, player.y)) {
                    moveBox(1, 0);

                    //CHECK IF AFTER MOVE PLAYER HAS WON
                    checkGameWin();
                }
                else {
                    player.x++;
                    player.moveTime = 0.f;
                }
            }
        }
    }
    else {
        player.moveTime = player.maxMoveTime;
    }
}

void update() {
    //MAP
    drawMap();
    //BOX
    drawBox();
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

int main()
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
        closeApplication();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    //INIT DEFAULT VALUES BEFORE WINDOW
    initialize(window);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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