/*
    Copyright (C) 2014 Parrot SA

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the 
      distribution.
    * Neither the name of Parrot nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/
/**
 * @file ihm.c
 * @brief This file contains sources about ncurses IHM used by arsdk example "JumpingSumoPiloting"
 * @date 15/01/2015
 */

/*****************************************
 *
 *             include file :
 *
 *****************************************/

#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <sys/time.h>

#include <libARSAL/ARSAL.h>

#include "ihm.h"

/*****************************************
 *
 *             define :
 *
 *****************************************/

#define HEADER_X 0
#define HEADER_Y 0

#define INFO_X 0
#define INFO_Y 2

#define BATTERY_X 0
#define BATTERY_Y 4

#define ATTITUDE_ROLL_X 0
#define ATTITUDE_ROLL_Y 6
#define ATTITUDE_PITCH_X 0
#define ATTITUDE_PITCH_Y 7
#define ATTITUDE_YAW_X 0
#define ATTITUDE_YAW_Y 8

#define SPEEDX_X 0
#define SPEEDX_Y 10
#define SPEEDY_X 0
#define SPEEDY_Y 11
#define SPEEDZ_X 0
#define SPEEDZ_Y 12

#define LATITUDE_X 0
#define LATITUDE_Y 14
#define LONGITUDE_X 0
#define LONGITUDE_Y 15
#define ALTITUDE_X 0
#define ALTITUDE_Y 16

#define COMMAND_X 0
#define COMMAND_Y 18

/*****************************************
 *
 *             private header:
 *
 ****************************************/
void *IHM_InputProcessing(void *data);

/*****************************************
 *
 *             implementation :
 *
 *****************************************/

IHM_t *IHM_New (IHM_onInputEvent_t onInputEventCallback)
{
    int failed = 0;
    IHM_t *newIHM = NULL;
    
    // check parameters
    if (onInputEventCallback == NULL)
    {
        failed = 1;
    }
    
    if (!failed)
    {
        //  Initialize IHM
        newIHM = malloc(sizeof(IHM_t));
        if (newIHM != NULL)
        {
            //  Initialize ncurses
            newIHM->mainWindow = initscr();
            newIHM->inputThread = NULL;
            newIHM->run = 1;
            newIHM->onInputEventCallback = onInputEventCallback;
            newIHM->customData = NULL;
        }
        else
        {
            failed = 1;
        }
    }
    
    if (!failed)
    {
        raw();                  // Line buffering disabled
        keypad(stdscr, TRUE);
        noecho();               // Don't echo() while we do getch
        timeout(100);
        
        refresh();
    }
    
    if (!failed)
    {
        //start input thread
        if(ARSAL_Thread_Create(&(newIHM->inputThread), IHM_InputProcessing, newIHM) != 0)
        {
            failed = 1;
        }
    }
    
    if (failed)
    {
        IHM_Delete (&newIHM);
    }

    return  newIHM;
}

void IHM_Delete (IHM_t **ihm)
{
    //  Clean up

    if (ihm != NULL)
    {
        if ((*ihm) != NULL)
        {
            (*ihm)->run = 0;
            
            if ((*ihm)->inputThread != NULL)
            {
                ARSAL_Thread_Join((*ihm)->inputThread, NULL);
                ARSAL_Thread_Destroy(&((*ihm)->inputThread));
                (*ihm)->inputThread = NULL;
            }
            
            delwin((*ihm)->mainWindow);
            (*ihm)->mainWindow = NULL;
            endwin();
            refresh();
            
            free (*ihm);
            (*ihm) = NULL;
        }
    }
}

void IHM_setCustomData(IHM_t *ihm, void *customData)
{
    if (ihm != NULL)
    {
        ihm->customData = customData;
    }
}

void *IHM_InputProcessing(void *data)
{
    IHM_t *ihm = (IHM_t *) data;
    int key = 0;

    struct timeval currentTime, beginAutomationTime;
    bool automationActive = false;

    if (ihm != NULL)
    {
        while (ihm->run)
        {
            key = getch();
            
            if ((key == 27) || (key =='q'))
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_EXIT, ihm->customData);
                }
            }
            else if(key == KEY_UP)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_UP, ihm->customData);
                }
            }
            else if(key == KEY_DOWN)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_DOWN, ihm->customData);
                }
            }
            else if(key == KEY_LEFT)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
                }
            }
            else if(key == KEY_RIGHT)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
                }
            }
            else if(key == 'e')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_EMERGENCY, ihm->customData);
                }
            }
            else if(key == 't')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF, ihm->customData);
                }
            }
            else if(key == ' ')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_LAND, ihm->customData);
                }
            }
            else if(key == 'r')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
                }
            }
            else if(key == 'f')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
                }
            }
            else if(key == 'd')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_ROLL_LEFT, ihm->customData);
                }
            }
            else if(key == 'g')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    automationActive = false;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_ROLL_RIGHT, ihm->customData);
                }
            }
            else if(key == 'o')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    gettimeofday(&beginAutomationTime, NULL);
                    automationActive = true;
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF, ihm->customData);
                }
            }
            else
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    if (automationActive == true)
                    {
                        gettimeofday(&currentTime, NULL);
                        if((currentTime.tv_usec - beginAutomationTime.tv_usec) < 2000)
                        {
                            ihm->onInputEventCallback (IHM_INPUT_EVENT_UP, ihm->customData);
                        }
                        if((currentTime.tv_usec - beginAutomationTime.tv_usec) >= 2000 && (currentTime.tv_usec - beginAutomationTime.tv_usec) < 6000)
                        {
                            ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
                        }
                        if((currentTime.tv_usec - beginAutomationTime.tv_usec) >= 6000)
                        {
                            ihm->onInputEventCallback (IHM_INPUT_EVENT_LAND, ihm->customData);
                            automationActive = false;
                        }
                    }
                    else
                    {
                        ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    } 
                }
            }
            
            usleep(10);
        }
    }
    
    return NULL;
}

void IHM_PrintHeader(IHM_t *ihm, char *headerStr)
{
    if (ihm != NULL)
    {
        move(HEADER_Y, 0);   // move to begining of line
        clrtoeol();          // clear line
        mvprintw(HEADER_Y, HEADER_X, headerStr);
    }
}

void IHM_PrintInfo(IHM_t *ihm, char *infoStr)
{
    if (ihm != NULL)
    {
        move(INFO_Y, 0);    // move to begining of line
        clrtoeol();         // clear line
        mvprintw(INFO_Y, INFO_X, infoStr);
    }
}

void IHM_PrintBattery(IHM_t *ihm, uint8_t percent)
{
    if (ihm != NULL)
    {
        move(BATTERY_Y, 0);     // move to begining of line
        clrtoeol();             // clear line
        mvprintw(BATTERY_Y, BATTERY_X, "Battery: %d", percent);
    }
}

void IHM_PrintAttitude(IHM_t *ihm, float roll, float pitch, float yaw)
{
    if (ihm != NULL)
    {
        move(ATTITUDE_ROLL_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(ATTITUDE_ROLL_Y, ATTITUDE_ROLL_X, "Roll: %.5f", roll);

        move(ATTITUDE_PITCH_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(ATTITUDE_PITCH_Y, ATTITUDE_PITCH_X, "Pitch: %.5f", pitch);

        move(ATTITUDE_YAW_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(ATTITUDE_YAW_Y, ATTITUDE_YAW_X, "Yaw: %.5f", yaw);
    }
}

void IHM_PrintSpeed(IHM_t *ihm, float speedX, float speedY, float speedZ)
{
    if (ihm != NULL)
    {
        move(SPEEDX_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(SPEEDX_Y, SPEEDX_X, "Speed X: %f", speedX);

        move(SPEEDY_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(SPEEDY_Y, SPEEDY_X, "Speed Y: %f", speedY);

        move(SPEEDZ_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(SPEEDZ_Y, SPEEDZ_X, "Speed Z: %f", speedZ);
    }
}

void IHM_PrintPosition(IHM_t *ihm, double latitude, double longitude, double altitude)
{
    if (ihm != NULL)
    {
        move(LATITUDE_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(LATITUDE_Y, LATITUDE_X, "Latitude: %d", latitude);

        move(LONGITUDE_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(LONGITUDE_Y, LONGITUDE_X, "Longitude: %d", longitude);

        move(ALTITUDE_Y, 0);     // move to begining of line
        clrtoeol();              // clear line
        mvprintw(ALTITUDE_Y, ALTITUDE_X, "Altitude: %d", altitude);
    }
}

void IHM_PrintCommand(IHM_t *ihm, int event)
{
    if (ihm != NULL)
    {
        move(COMMAND_Y, 0);     // move to begining of line
        clrtoeol();             // clear line
        switch (event)
        {
            case 3:
                mvprintw(COMMAND_Y, COMMAND_X, "Command: TAKEOFF");
                break;
            case 4:
                mvprintw(COMMAND_Y, COMMAND_X, "Command: LAND");
                break;
            case 5:
                mvprintw(COMMAND_Y, COMMAND_X, "Command: UP");
                break;
            case 9:
                mvprintw(COMMAND_Y, COMMAND_X, "Command: FORWARD");
                break;
            default:
                mvprintw(COMMAND_Y, COMMAND_X, "Command: ....");
                break;
        }
        
    }
}
