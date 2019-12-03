#include "voxelquest/gamestate.h"
#include "voxelquest/enums.h"

int GameState::tbTicks;
FIVector4 GameState::origin;

float GameState::subjectZoom=1.0f;
float GameState::targetSubjectZoom=1.0f;
bool GameState::markerFound=false;
FIVector4 GameState::worldMarker;

bool GameState::refreshPaths=false;

float GameState::MAX_GPU_MEM=4096.0f;
float GameState::VERTEX_MEM_USAGE=0.0f;
float GameState::TOT_GPU_MEM_USAGE=0.0f;

float GameState::MAX_CPU_MEM=4096.0f;
float GameState::TOT_CPU_MEM_USAGE=0.0f;

int GameState::TOT_POINT_COUNT=0;

long long GameState::ENT_COUNTER=0;

double GameState::timeDelta;
double GameState::curTime;
float GameState::smoothTime=0.0f;
double GameState::pauseTime;
double GameState::clickTime;
double GameState::mdTime;
double GameState::muTime;

int GameState::pathFindingStep=0;
FIVector4 GameState::lastHolderPos;

std::vector<RotationInfo> GameState::rotMatStack;
std::vector<DynObject *> GameState::dynObjects;

GameWorld* GameState::gw=nullptr;
GameEntManager* GameState::gem=nullptr;
GamePhysics* GameState::gamePhysics=nullptr;
GameFluid* GameState::gameFluid[E_FID_LENGTH];
GameLogic* GameState::gameLogic=nullptr;
GameNetwork* GameState::gameNetwork=nullptr;
GameAI* GameState::gameAI=nullptr;
GameGUI *GameState::ui=nullptr;

GameState *GameState::singleton()
{
    static GameState gameState;

    return &gameState;
}

GameState::GameState()
{
	init();
}

void GameState::init()
{
	tbTicks=0;

    m_wsBufferInvalid=true;
    m_forceGetPD=false;
    m_forceShadowUpdate=0;

    for(i=0; i<E_OBJ_LENGTH; i++)
    {
        dynObjects.push_back(new DynObject());
    }

    dynObjects[E_OBJ_CAMERA]->init(0, 0, 0, 0, 0, 0, false, E_MT_NONE, NULL, 4.0f);

    for(i=E_OBJ_LIGHT0; i<E_OBJ_LENGTH; i++)
    {

        j=i-E_OBJ_LIGHT0;

        switch(j)
        {
        case 0:
            ccr=255;
            ccg=255;
            ccb=255;
            break;

        case 1:
            ccr=255;
            ccg=0;
            ccb=0;

            break;

        case 2:
            ccr=255;
            ccg=128;
            ccb=0;

            break;

        case 3:
            ccr=0;
            ccg=255;
            ccb=0;

            break;

        case 4:
            ccr=0;
            ccg=128;
            ccb=255;

            break;
        case 5:
            ccr=128;
            ccg=0;
            ccb=255;

            break;
        }

        if(i==E_OBJ_LIGHT0)
        {
            tempf=128.0f;
        }
        else
        {
            tempf=16.0f;
        }

        dynObjects[i]->init(
            -16+i*2,
            -16+i*2,
            4,
            ccr, ccg, ccb,
            true,
            E_MT_RELATIVE,
            &(dynObjects[E_OBJ_CAMERA]->pos),
            0.125f,
            tempf
        );
    }

    dynObjects[E_OBJ_LIGHT0]->moveType=E_MT_TRACKBALL;
    cameraPos=&(dynObjects[E_OBJ_CAMERA]->pos);
}

void GameState::stopAllThreads()
{
	glFlush();
	glFinish();
	gameLogic->threadPoolPath->stopAll();
	gameLogic->threadPoolList->stopAll();
	glFlush();
	glFinish();
}

