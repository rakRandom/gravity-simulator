#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <time.h>
#include "raylib/raylib.h"
#include "raylib/raymath.h"


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    srand(time(0));

    const double gravityForce = 5000000;
    const double gravityPointSpeed = 50;
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
    double gravityDirection = 1;

    // Dots variables
    size_t vectorListSize = 200;
    Vector2 *vectorList = (Vector2*) std::malloc(vectorListSize * sizeof(Vector2));
    Vector2 *velocityList = (Vector2*) std::malloc(vectorListSize * sizeof(Vector2));

    for (size_t i = 0; i < vectorListSize; i++) 
    {
        vectorList[i] = Vector2{(float) (rand() % screenWidth) * 100, (float) (rand() % screenHeight) * 100};
        velocityList[i] = Vector2Zero();
    }

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Event Get
        //----------------------------------------------------------------------------------
        // Toggle to show general details
        if (IsKeyReleased(KEY_INSERT))
            showGeneralDetails = (uint8_t) (showGeneralDetails == 0);
        
        // Toggle to show gravity details
        if (IsKeyReleased(KEY_DELETE))
            showGravityDetails = (uint8_t) (showGravityDetails == 0);
        
        // Toggles if the gravity will attract or repel
        if (IsKeyReleased(KEY_R))
            gravityDirection *= -1;

        // Toggle if the gravity point is based on the mouse position or not
        if (IsKeyReleased(KEY_P)) 
        {
            useMouseAsGravityPoint = (uint8_t) (useMouseAsGravityPoint == 0);
            gravityPoint = Vector2{screenWidth / 2, screenHeight / 2};
        }

        // Toggle Dark Mode
        if (IsKeyReleased(KEY_K))
            useDarkMode = (uint8_t) (useDarkMode == 0);

        //----------------------------------------------------------------------------------

        // Update
        //----------------------------------------------------------------------------------

        // Mouse-related gravity point calculation
        if (useMouseAsGravityPoint)
            gravityPoint = GetMousePosition();
        
        // Not-mouse-related gravity point movement
        else 
        {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) 
            {
                gravityPoint.x -= gravityPointSpeed * GetFrameTime();

                if (gravityPoint.x < screenWidth / 2 - gravityPointSpeed)
                    gravityPoint.x = screenWidth / 2 + gravityPointSpeed;
            }
            else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) 
            {
                gravityPoint.x += gravityPointSpeed * GetFrameTime();

                if (gravityPoint.x > screenWidth / 2 + gravityPointSpeed)
                    gravityPoint.x = screenWidth / 2 - gravityPointSpeed;
            }
            else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
                gravityPoint.x = screenWidth / 2;
        }
        
        

        // Velocity calculation
        for (size_t i = 0; i < vectorListSize; i++) 
        {
            scaledGravityPoint = Vector2Scale(gravityPoint, 100);
            distance = Vector2Distance(scaledGravityPoint, vectorList[i]);

            if (!(distance < 99 && distance > -99))
            {
                move = Vector2MoveTowards(vectorList[i], scaledGravityPoint, 1);

                velocityList[i] = Vector2Add(
                    Vector2Scale(
                        Vector2Normalize(
                            Vector2Subtract(
                                move, 
                                vectorList[i]
                            )
                        ), 
                        (gravityForce * GetFrameTime() * gravityDirection) / (distance / 2)
                    ),
                    velocityList[i]
                ); 

                if (velocityList[i].x > 5000)
                    velocityList[i].x = 5000;
                else if (velocityList[i].x < -5000)
                    velocityList[i].x = -5000;
                
                if (velocityList[i].y > 5000)
                    velocityList[i].y = 5000;
                else if (velocityList[i].y < -5000)
                    velocityList[i].y = -5000;
            }
        }

        // Position update
        for (size_t i = 0; i < vectorListSize; i++) 
        {
            vectorList[i] = Vector2Add(vectorList[i], Vector2Scale(velocityList[i], GetFrameTime()));
        }
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            
            if (useDarkMode)
            {
                ClearBackground(BLACK);

                for (size_t i = 0; i < vectorListSize; i++) 
                    DrawCircleV(Vector2Scale(vectorList[i], 0.01), 2.0f, Color{(unsigned char) (Vector2Length(velocityList[i]) * 235 / 7100 + 20), 0, 0, 255});
                
                DrawText("DARK MODE: ON", screenWidth - 140, 5, 18, GREEN);
            }
            else
            {
                ClearBackground(RAYWHITE);

                for (size_t i = 0; i < vectorListSize; i++) 
                    DrawCircleV(Vector2Scale(vectorList[i], 0.01), 2.0f, BLACK);
            }

            // Draw the dots
            DrawCircleV(gravityPoint, 2.0f, BLUE);

            // Draw the details
            if (showGeneralDetails)
            {
                // General details
                DrawText(
                    std::string("FPS: ").append(std::to_string(GetFPS())).c_str(), 
                    5, 5, 18, GREEN
                );
                DrawText(
                    std::string("DOTS: ").append(std::to_string(vectorListSize)).c_str(), 
                    5, 25, 18, GREEN
                );
            }
            if (showGravityDetails)
            {
                // Gravity details
                DrawText(
                    std::string("GRAV. FORCE: ").append(std::to_string(gravityForce)).c_str(), 
                    5, 55, 18, GREEN
                );
                DrawText(
                    std::string("GRAV. REVERSED: ").append((gravityDirection == 1) ? "FALSE" : "TRUE").c_str(), 
                    5, 75, 18, GREEN
                );
                DrawText(
                    std::string("GRAV. POS.").c_str(), 
                    5, 95, 18, GREEN
                );
                DrawText(
                    std::string("X: ").append(std::to_string(gravityPoint.x)).c_str(), 
                    5, 115, 18, GREEN
                );
                DrawText(
                    std::string("Y: ").append(std::to_string(gravityPoint.y)).c_str(), 
                    5, 135, 18, GREEN
                );

                if (!useMouseAsGravityPoint)
                    DrawText(
                        std::string("GRAV. POINT SPEED: ").append(std::to_string(gravityPointSpeed)).c_str(), 
                        5, 155, 18, GREEN
                    );
            }

            // Draw the keys
            DrawText(
                "INSERT - Info. 1 Toggle | DELETE - Info. 2 Toggle | R - Grav. Force Toggle | P - Grav. Point Toggle | K - Theme Toggle",
                5,
                screenHeight - 14,
                14,
                GREEN
            );
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(vectorList);
    free(velocityList);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
