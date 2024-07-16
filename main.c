#include "stdio.h"
#include "stdlib.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"

int done = 0;


int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }

    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_centre_sensor=0, left_sensor=0, right_sensor=0;
    clock_t start_time=0, end_time;
    int msec;
    int crashed = 0;

    // SETUP MAZE
    int name_index = 0;
    insertAndSetFirstWall(&head, name_index++,  150, 450, 10, 50);
    insertAndSetFirstWall(&head, name_index++,  250, 350, 10, 150);
    insertAndSetFirstWall(&head, name_index++,  50, 450, 100, 10);
    insertAndSetFirstWall(&head, name_index++,  150, 350, 100, 10);
    insertAndSetFirstWall(&head, name_index++,  50, 230, 10, 220);
    insertAndSetFirstWall(&head, name_index++,  150, 330, 10, 30);
    insertAndSetFirstWall(&head, name_index++,  50, 230, 10, 170);
    insertAndSetFirstWall(&head, name_index++,  150, 330, 10, 30);
    insertAndSetFirstWall(&head, name_index++,  50, 230, 330, 10);
    insertAndSetFirstWall(&head, name_index++,  150, 330, 130, 10);
    insertAndSetFirstWall(&head, name_index++,  380, 230, 10, 100);
    insertAndSetFirstWall(&head, name_index++,  280, 330, 10, 100);
    insertAndSetFirstWall(&head, name_index++,  280, 430, 110, 10);


    insertAndSetFirstWall(&head, name_index++,  380, 430, 50, 10);

    insertAndSetFirstWall(&head, name_index++,  110, 30, 310, 10);
    insertAndSetFirstWall(&head, name_index++,  10, 130, 380, 10);
    insertAndSetFirstWall(&head, name_index++,  110, 0, 10, 30);
    insertAndSetFirstWall(&head, name_index++,  10, 0, 10, 130);

    int i;
    for (i = 0; i < 200; i++){
        insertAndSetFirstWall(&head, name_index++,  620 - i , 230 + i, 10, 10); //1
        if (i>100)
            insertAndSetFirstWall(&head, name_index++,  580 - i , 130 + i, 10, 10); //1
    }
    for (i = 0; i < 200; i++){
        insertAndSetFirstWall(&head, name_index++,  420 + i , 30 + i, 10, 10); //1
        if (i>100)
            insertAndSetFirstWall(&head, name_index++,  280 + i , 30 + i, 10, 10); //1
    }

    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;
    while(!done){
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        //Move robot based on user input commands/auto commands
        if (robot.auto_mode == 1)
            robotAutoMotorMove(&robot, front_centre_sensor, left_sensor, right_sensor);
        robotMotorMove(&robot, crashed);

        //Check if robot reaches endpoint. and check sensor values
        if (checkRobotReachedEnd(&robot, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT/2+100, 10, 100)){
            end_time = clock();
            msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
            robotSuccess(&robot, msec);
        }
        else if(crashed == 1 || checkRobotHitWalls(&robot, head)){
            robotCrash(&robot);
            crashed = 1;
        }
        //Otherwise compute sensor information
        else {
            front_centre_sensor = checkRobotSensorFrontCentreAllWalls(&robot, head);
            if (front_centre_sensor>0)
                printf("Getting close on the centre. Score = %d\n", front_centre_sensor);

            left_sensor = checkRobotSensorLeftAllWalls(&robot, head);
            if (left_sensor>0)
                printf("Getting close on the left. Score = %d\n", left_sensor);

            right_sensor = checkRobotSensorRightAllWalls(&robot, head);
            if (right_sensor>0)
                printf("Getting close on the right. Score = %d\n", right_sensor);
        }
        robotUpdate(renderer, &robot);
        updateAllWalls(head, renderer);

        // Check for user input
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                robot.direction = UP;
            }
            if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                robot.direction = DOWN;
            }
            if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
                robot.direction = LEFT;
            }
            if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
                robot.direction = RIGHT;
            }
            if(state[SDL_SCANCODE_SPACE]){
                setup_robot(&robot);
            }
            if(state[SDL_SCANCODE_RETURN]){
                robot.auto_mode = 1;
                start_time = clock();
            }
        }

        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
    return(1);
}
