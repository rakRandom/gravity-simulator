#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <time.h>
#include "raylib/raylib.h"
#include "raylib/raymath.h"

struct Dot
{
    Vector2 position;
    Vector2 velocity;
} Dot;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(0));

    const double gravityForce = 5000000;
    const double gravityPointSpeed = 50;
    const double speedLimit = 7500;
    const int screenWidth = 1080;
    const int screenHeight = 720;
    const std::string title("RayLib - Gravity Simulator");

    InitWindow(screenWidth, screenHeight, title.c_str());

    // Boolean variables
    uint8_t showGeneralDetails = 1;
    uint8_t showGravityDetails = 1;
    uint8_t useMouseAsGravityPoint = 0;
    uint8_t useDarkMode = 0;

    // Gravity calculation variables
    Vector2 gravityPoint = Vector2{screenWidth / 2, screenHeight / 2};
    Vector2 scaledGravityPoint;
    Vector2 move;
    double distance;
    double gravityDirection = 0;

    // Dots variables
    size_t dotQuantity;
    if (argc == 1)
        dotQuantity = 100;
    else
        dotQuantity = std::stoi(argv[1]);

    struct Dot *dotArray = (struct Dot *)std::malloc(dotQuantity * sizeof(struct Dot));

    for (size_t i = 0; i < dotQuantity; i++)
    {
        dotArray[i].position = Vector2{(float)(rand() % screenWidth) * 100, (float)(rand() % screenHeight) * 100};
        dotArray[i].velocity = Vector2Zero();
    }

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Event Get
        //----------------------------------------------------------------------------------
        // Toggle to show general details
        if (IsKeyReleased(KEY_INSERT))
            showGeneralDetails = (uint8_t)(showGeneralDetails == 0);

        // Toggle to show gravity details
        if (IsKeyReleased(KEY_DELETE))
            showGravityDetails = (uint8_t)(showGravityDetails == 0);

        // Toggles the gravity mode - -1 to 0 to 1 to -1
        if (IsKeyReleased(KEY_R))
            gravityDirection = (gravityDirection == 0) ? 1 : ((gravityDirection == 1) ? -1 : 0);

        // Toggle if the gravity point is based on the mouse position or not
        if (IsKeyReleased(KEY_P))
        {
            useMouseAsGravityPoint = (uint8_t)(useMouseAsGravityPoint == 0);
            gravityPoint = Vector2{screenWidth / 2, screenHeight / 2};
        }

        // Toggle Dark Mode
        if (IsKeyReleased(KEY_K))
            useDarkMode = (uint8_t)(useDarkMode == 0);

        //----------------------------------------------------------------------------------

        // Update
        //----------------------------------------------------------------------------------

        // Mouse-related gravity point calculation
        if (useMouseAsGravityPoint)
            gravityPoint = GetMousePosition();

        // Not-mouse-related gravity point movement
        else
        {
            // Horizontal movement
            if (IsKeyDown(KEY_A))
                gravityPoint.x -= gravityPointSpeed * GetFrameTime();
            if (IsKeyDown(KEY_D))
                gravityPoint.x += gravityPointSpeed * GetFrameTime();

            // Vertical movement
            if (IsKeyDown(KEY_W))
                gravityPoint.y -= gravityPointSpeed * GetFrameTime();
            if (IsKeyDown(KEY_S))
                gravityPoint.y += gravityPointSpeed * GetFrameTime();

            // Go back to default position
            if (IsKeyDown(KEY_H))
                gravityPoint.x = screenWidth / 2;
        }

        // Velocity calculation
        for (size_t i = 0; i < dotQuantity; i++)
        {
            scaledGravityPoint = Vector2Scale(gravityPoint, 100);
            distance = Vector2Distance(scaledGravityPoint, dotArray[i].position);

            if (!(distance < 99 && distance > -99))
            {
                move = Vector2MoveTowards(dotArray[i].position, scaledGravityPoint, 1);

                dotArray[i].velocity = Vector2Add(
                    Vector2Scale(
                        Vector2Normalize(
                            Vector2Subtract(
                                move,
                                dotArray[i].position)),
                        (gravityForce * GetFrameTime() * gravityDirection) / (distance / 2)),
                    dotArray[i].velocity);

                if (Vector2Length(dotArray[i].velocity) > speedLimit)
                    dotArray[i].velocity = Vector2Scale(Vector2Normalize(dotArray[i].velocity), speedLimit);
            }
        }

        // Position update
        for (size_t i = 0; i < dotQuantity; i++)
        {
            dotArray[i].position = Vector2Add(dotArray[i].position, Vector2Scale(dotArray[i].velocity, GetFrameTime()));
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        if (useDarkMode)
        {
            ClearBackground(BLACK);

            for (size_t i = 0; i < dotQuantity; i++)
                DrawCircleV(Vector2Scale(dotArray[i].position, 0.01), 2.0f, Color{(unsigned char)(Vector2Length(dotArray[i].velocity) * 200 / speedLimit + 55), 0, 0, 255});

            DrawText("DARK MODE: ON", screenWidth - 140, 5, 18, GREEN);
        }
        else
        {
            ClearBackground(RAYWHITE);

            for (size_t i = 0; i < dotQuantity; i++)
                DrawCircleV(Vector2Scale(dotArray[i].position, 0.01), 2.0f, BLACK);
        }

        // Draw the dots
        DrawCircleV(gravityPoint, 2.0f, BLUE);

        // Draw the details
        if (showGeneralDetails)
        {
            // General details
            DrawText(
                std::string("FPS: ").append(std::to_string(GetFPS())).c_str(),
                5, 5, 18, GREEN);
            DrawText(
                std::string("DOTS: ").append(std::to_string(dotQuantity)).c_str(),
                5, 25, 18, GREEN);
        }
        if (showGravityDetails)
        {
            // Gravity details
            DrawText(
                std::string("GRAV. FORCE: ").append(std::to_string(gravityForce)).c_str(),
                5, 55, 18, GREEN);
            DrawText(
                std::string("GRAV. REVERSED: ").append((gravityDirection == 0) ? "DISABLED" : ((gravityDirection == 1) ? "NORMAL" : "REVERSED")).c_str(),
                5, 75, 18, GREEN);
            DrawText(
                std::string("GRAV. POS.").c_str(),
                5, 95, 18, GREEN);
            DrawText(
                std::string("X: ").append(std::to_string(gravityPoint.x)).c_str(),
                5, 115, 18, GREEN);
            DrawText(
                std::string("Y: ").append(std::to_string(gravityPoint.y)).c_str(),
                5, 135, 18, GREEN);

            if (!useMouseAsGravityPoint)
                DrawText(
                    std::string("GRAV. POINT SPEED: ").append(std::to_string(gravityPointSpeed)).c_str(),
                    5, 155, 18, GREEN);
        }

        // Draw the keys
        DrawText(
            "INSERT - Info. 1 Toggle | DELETE - Info. 2 Toggle | R - Grav. Mode Toggle | P - Grav. Point Toggle | K - Theme Toggle",
            5,
            screenHeight - 14,
            14,
            GREEN);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(dotArray);
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
