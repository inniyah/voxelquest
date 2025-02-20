#include "voxelquest/gamenetwork.h"
#include "voxelquest/gamestate.h"
#include "voxelquest/gameentmanager.h"
#include "voxelquest/bullethelpers.h"
#include "voxelquest/renderer.h"

#ifdef _WIN32
#include <Ws2tcpip.h>

#else
#define closesocket close

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define INVALID_SOCKET ((SOCKET)(~0))
#define SOCKET_ERROR   (-1)

#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#endif 

#include <iostream>
#include <cstring>
#include <cstdio>

static void ClearWinSock() {
#if defined WIN32
	WSACleanup();
#endif
}

GameNetwork::GameNetwork()
{

}

void GameNetwork::init(Singleton* _singleton)
{
    singleton=_singleton;
    ConnectSocket=INVALID_SOCKET;
    isConnected=false;

    recvPosInBytes=0;
    sendPosInBytes=0;

    recvConsumedInBytes=0;
    sendConsumedInBytes=0;

}


void GameNetwork::getIntFloatLen(int opCode, int* uintLen, int* intLen, int* floatLen)
{

    *uintLen=0;
    *intLen=0;
    *floatLen=0;


    switch(opCode)
    {
    case E_NO_TERMINAL:

        break;
    case E_NO_KEY_ACTION:
        *uintLen=1;
        *intLen=1;
        *floatLen=2;
        break;
    case E_NO_ADD_ENT:
        *intLen=1;
        *floatLen=3;
        break;
    case E_NO_REM_ENT:
        *intLen=1;
        break;
    case E_NO_DRAG_ENT:
        *intLen=4;
        *floatLen=3;
        break;
    default:
        std::cout<<"Unsupported net operation\n";
        break;

    }
}


void GameNetwork::addNetworkAction(
    int opCode,
    uint* naUintData,
    int* naIntData,
    float* naFloatData
)
{
    char* curDataPtr;

    int i;
    int pos;

    int uintLen=0;
    int intLen=0;
    int floatLen=0;
    getIntFloatLen(opCode, &uintLen, &intLen, &floatLen);

    if(!isConnected)
    {
        return;
    }

    netSendStack.push_back(NetworkAction());
    curDataPtr=&(netSendStack.back().data[0]);

    curDataPtr[0]=opCode;

    pos=4;

    for(i=0; i<uintLen; i++)
    {
        uintPtr[i]=toUintPtr(&(curDataPtr[pos])); pos+=4;
        *(uintPtr[i])=naUintData[i];
    }
    for(i=0; i<intLen; i++)
    {
        intPtr[i]=toIntPtr(&(curDataPtr[pos])); pos+=4;
        *(intPtr[i])=naIntData[i];
    }
    for(i=0; i<floatLen; i++)
    {
        floatPtr[i]=toFloatPtr(&(curDataPtr[pos])); pos+=4;
        *(floatPtr[i])=naFloatData[i];
    }


}

void GameNetwork::addNetworkActionForSend(
    NetworkAction* na
)
{
    char* curDataPtr;
    int i;

    curDataPtr=&(sendbuf[sendPosInBytes]);

    for(i=0; i<NA_SIZE_IN_BYTES; i++)
    {
        curDataPtr[i]=na->data[i];
    }

    sendPosInBytes+=NA_SIZE_IN_BYTES;

}

void GameNetwork::applyNetworkActions()
{


    int i;
    int pos;
    int opCode;
    char* curDataPtr;

    while((recvPosInBytes-recvConsumedInBytes)>=NA_SIZE_IN_BYTES)
    {


        curDataPtr=&(recvbuf[recvConsumedInBytes]);

        pos=4;

        opCode=curDataPtr[0];

        int intLen=0;
        int floatLen=0;
        int uintLen=0;
        getIntFloatLen(opCode, &uintLen, &intLen, &floatLen);

        for(i=0; i<uintLen; i++)
        {
            uintPtr[i]=toUintPtr(&(curDataPtr[pos])); pos+=4;
        }
        for(i=0; i<intLen; i++)
        {
            intPtr[i]=toIntPtr(&(curDataPtr[pos])); pos+=4;
        }
        for(i=0; i<floatLen; i++)
        {
            floatPtr[i]=toFloatPtr(&(curDataPtr[pos])); pos+=4;

            if(i<4)
            {
                tempVecs[0].setIndex(i, *(floatPtr[i]));
            }
            else
            {
                tempVecs[1].setIndex(i-4, *(floatPtr[i]));
            }
        }

        switch(opCode)
        {
        case E_NO_TERMINAL:

            break;
        case E_NO_KEY_ACTION:
            applyKeyAction(
                false,
                *(intPtr[0]),
                *(uintPtr[0]),
                *(floatPtr[0]),
                *(floatPtr[1])
            );
            break;
        case E_NO_ADD_ENT:
            GameState::gem->placeNewEnt(false, *(intPtr[0]), &(tempVecs[0]));
            break;
        case E_NO_REM_ENT:
            GameState::gem->removeEntity(false, *(intPtr[0]));
            break;
        case E_NO_DRAG_ENT:
            GameState::gem->performDrag(
                false,
                *(intPtr[0]),
                *(intPtr[1]),
                *(intPtr[2]),
                *(intPtr[3]),
                &(tempVecs[0])
            );
            break;

        }


        recvConsumedInBytes+=NA_SIZE_IN_BYTES;

    }

}


int GameNetwork::socketConnect(bool doConnect)
{
    int iResult;

    struct addrinfo *result=NULL,
        *ptr=NULL,
        hints;

    if(doConnect)
    {
#ifdef _WIN32
        // Initialize Winsock
        iResult=WSAStartup(MAKEWORD(2, 2), &wsaData);
        if(iResult!=0)
        {
            printf("WSAStartup failed with error: %d\n", iResult);
            return 1;
        }
#endif

        memset(&hints, 0, sizeof(hints));
        hints.ai_family=AF_UNSPEC;
        hints.ai_socktype=SOCK_STREAM;
        hints.ai_protocol=IPPROTO_TCP;

        // Resolve the server address and port
        iResult=getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result); //argv[1] //localhost
        if(iResult!=0)
        {
            printf("getaddrinfo failed with error: %d\n", iResult);
            ClearWinSock();
            return 1;
        }

        // Attempt to connect to an address until one succeeds
        for(ptr=result; ptr!=NULL; ptr=ptr->ai_next)
        {

            // Create a SOCKET for connecting to server
            ConnectSocket=socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if(ConnectSocket==INVALID_SOCKET)
            {
#ifdef _WIN32
                printf("socket failed with error: %ld\n", WSAGetLastError());
#else
                printf("socket failed with error: %ld: %s\n", errno, strerror(errno));
#endif
                ClearWinSock();
                return 1;
            }

            // Connect to server.
            iResult=connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if(iResult==SOCKET_ERROR)
            {
                closesocket(ConnectSocket);
                ConnectSocket=INVALID_SOCKET;
                continue;
            }
            break;
        }

        freeaddrinfo(result);

        if(ConnectSocket==INVALID_SOCKET)
        {
            printf("Unable to connect to server!\n");
            ClearWinSock();
            return 1;
        }

        isConnected=true;

    }
    else
    {
        // shutdown the connection since no more data will be sent
        iResult=shutdown(ConnectSocket, SD_SEND);
        if(iResult==SOCKET_ERROR)
        {
#ifdef _WIN32
            printf("shutdown failed with error: %ld\n", WSAGetLastError());
#else
            printf("shutdown failed with error: %ld: %s\n", errno, strerror(errno));
#endif
            closesocket(ConnectSocket);
            ClearWinSock();
            return 1;
        }




        // cleanup
        closesocket(ConnectSocket);
        ClearWinSock();

        isConnected=false;
    }




    return 0;

}


void GameNetwork::socketRecv()
{

    int iResult;

    iResult=recv(
        ConnectSocket,
        &(recvbuf[recvPosInBytes]),
        FRAME_SIZE_IN_BYTES,
        0
    );
    if(iResult>0)
    {
        recvPosInBytes+=iResult;
        //printf("Bytes received: %d\n", iResult);
    }
    else if(iResult==0)
    {
        printf("Connection closed\n");
    }
    else
    {
#ifdef _WIN32
        printf("recv failed with error: %ld\n", WSAGetLastError());
#else
        printf("recv failed with error: %ld: %s\n", errno, strerror(errno));
#endif
    }
}

void GameNetwork::socketSend()
{//char* bufToSend, int totSize) {
//sendbuf, sendPosInBytes - sendConsumedInBytes

    int iResult;

    //cout << "Sending...\n";

    // Send an initial buffer
    iResult=send(
        ConnectSocket,
        &(sendbuf[sendConsumedInBytes]),
        sendPosInBytes-sendConsumedInBytes,
        0
    );

    if(iResult>=0)
    {
        sendConsumedInBytes+=
            sendPosInBytes-sendConsumedInBytes;
        //iResult;
    }

    if(iResult==SOCKET_ERROR)
    {
#ifdef _WIN32
        printf("send failed with error: %ld\n", WSAGetLastError());
#else
        printf("send failed with error: %ld: %s\n", errno, strerror(errno));
#endif
        closesocket(ConnectSocket);
        ClearWinSock();
    }

    //printf("Bytes Sent: %ld\n", iResult);


}

void GameNetwork::checkBufferLengthSend()
{

    int roundedPos;
    int basePos;
    int i;

    if(sendConsumedInBytes>(TOT_BUFFER_SIZE/2))
    {
        roundedPos=(sendConsumedInBytes/NA_SIZE_IN_BYTES)*NA_SIZE_IN_BYTES-NA_SIZE_IN_BYTES;

        basePos=sendConsumedInBytes-roundedPos;

        for(i=0; i<=(sendPosInBytes-sendConsumedInBytes); i++)
        {
            sendbuf[basePos+i]=sendbuf[sendConsumedInBytes+i];
        }

        sendConsumedInBytes-=roundedPos;
        sendPosInBytes-=roundedPos;
    }

}

void GameNetwork::checkBufferLengthRecv()
{

    int roundedPos;
    int basePos;
    int i;


    if(recvConsumedInBytes>(TOT_BUFFER_SIZE/2))
    {
        roundedPos=(recvConsumedInBytes/NA_SIZE_IN_BYTES)*NA_SIZE_IN_BYTES-NA_SIZE_IN_BYTES;

        basePos=recvConsumedInBytes-roundedPos;

        for(i=0; i<=(recvPosInBytes-recvConsumedInBytes); i++)
        {
            recvbuf[basePos+i]=recvbuf[recvConsumedInBytes+i];
        }

        recvConsumedInBytes-=roundedPos;
        recvPosInBytes-=roundedPos;
    }

}

void GameNetwork::flushNetworkActions()
{

    int i;

    // if nothing to send, add in an empty action
    if(
        ((sendPosInBytes-sendConsumedInBytes)==0)&&
        (netSendStack.size()==0)
        )
    {
        addNetworkAction(E_NO_TERMINAL, NULL, NULL, NULL);
    }

    //cout << "netSendStack.size() " << netSendStack.size() << "\n";

    for(i=0; i<netSendStack.size(); i++)
    {
        addNetworkActionForSend(&(netSendStack[i]));
    }

    netSendStack.clear();
}


void GameNetwork::updateSend()
{
    checkBufferLengthSend();
    socketSend();
}
void GameNetwork::updateRecv()
{
    checkBufferLengthRecv();
    socketRecv();
}

void GameNetwork::applyKeyAction(bool isReq, int actorId, uint keyFlags, float camRotX, float camRotY)
{
    bool strafeMode=GameState::gem->firstPerson;

//    int i;
    BaseObj* ca;

    btVector3 mouseWP;

//    bool charMoved;
    float deltaAng;

    if(isReq)
    {
        naUintData[0]=keyFlags;
        naIntData[0]=actorId;
        naFloatData[0]=camRotX;
        naFloatData[1]=camRotY;
        addNetworkAction(E_NO_KEY_ACTION, naUintData, naIntData, naFloatData);
        return;
    }

    if(g_settings.settings[E_BS_TURN_BASED])
    {
        return;
    }

    if(actorId<0)
    {

    }
    else
    {

        ca=&(GameState::gem->gameObjects[actorId]);

        unzipBits(keyFlags, keyMapResultUnzipped, KEYMAP_LENGTH);

        if(ca->isAlive())
        {

            if(keyMapResultUnzipped[KEYMAP_RIGHT])
            {

                if(strafeMode)
                {
                    GameState::gem->makeMove(actorId, btVector3(1.0f, 0.0f, 0.0f), true, true);
                }
                else
                {
                    GameState::gem->makeTurn(actorId, -getConst(E_CONST_TURN_AMOUNT));
                }

                //
            }

            if(keyMapResultUnzipped[KEYMAP_LEFT])
            {
                if(strafeMode)
                {
                    GameState::gem->makeMove(actorId, btVector3(-1.0f, 0.0f, 0.0f), true, true);
                }
                else
                {
                    GameState::gem->makeTurn(actorId, getConst(E_CONST_TURN_AMOUNT));
                }

                //
            }

            if(keyMapResultUnzipped[KEYMAP_FIRE_PRIMARY])
            {
                GameState::gem->makeShoot(actorId, E_ENTTYPE_BULLET);
            }

            if(keyMapResultUnzipped[KEYMAP_GRAB])
            {
                GameState::gem->makeGrab(actorId, -1);
            }

            if(keyMapResultUnzipped[KEYMAP_THROW])
            {
                GameState::gem->makeThrow(actorId, -1);
            }

            if(keyMapResultUnzipped[KEYMAP_UP])
            {
                GameState::gem->makeJump(actorId, 1, 1.0f);
            }

            if(keyMapResultUnzipped[KEYMAP_DOWN])
            {
                GameState::gem->makeJump(actorId, 0, 1.0f);
            }

            if(keyMapResultUnzipped[KEYMAP_FORWARD])
            {
                GameState::gem->makeMove(actorId, btVector3(0.0f, 1.0f, 0.0f), true, true);
            }

            if(keyMapResultUnzipped[KEYMAP_BACKWARD])
            {
                GameState::gem->makeMove(actorId, btVector3(0.0f, -1.0f, 0.0f), true, true);
            }

            // mouseWP = screenToWorld(
            // 	((float)lastPosX)/origWinW,
            // 	((float)lastPosY)/origWinH,
            // 	camRotX
            // );

            if(strafeMode)
            {
                deltaAng=ca->turnTowardsPointDelta(

                    ca->getCenterPoint(E_BDG_CENTER)+
                    convertToBTV(Renderer::lookAtVec)

                    // ca->getCenterPoint(E_BDG_CENTER) - mouseWP

                );

                if(!g_settings.settings[E_BS_EDIT_POSE])
                {
                    GameState::gem->makeTurn(actorId, deltaAng*16.0f);
                }
            }





        }
    }




}
