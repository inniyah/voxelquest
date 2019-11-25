#ifndef _voxelquest_gameentmanager_h_
#define _voxelquest_gameentmanager_h_

class GameEntManager
{
public:
    GameEntManager();

    void init(Singleton* _singleton);

    BaseObj* getCurActor();

    BaseObj* getActiveActor();


    void refreshActiveId();

    void endHumanTurn();

    void cycleTurn();

    void nextTurn();

    void refreshTurnList();

    void setTurnBased(bool newVal);

    void checkActorRefresh();

    void closeAllContainers();

    bool anyContainerOpen();

    void togglePoseEdit();

    void loadDefaultPose(int actorId);


    void applyNonPoseData();

    void setFirstPerson(bool _newVal);

    int getCurActorUID();

    void updateOrgMat(UIComponent* comp);

    void doDrag();

    void endDrag(int upInd);


    bool handleGUI(
        UIComponent* comp,
        bool mouseUpEvent,
        bool mouseDownEvent,
        bool noTravel,
        bool wasDoubleClick
    );



    BaseObj* getEquipped(BaseObj* parentObj);

    void updateDragInfo(int bestInd, bool lbDown, bool wasDoubleClick);


    int getRandomContId();
    int getRandomNPCId();
    int getRandomMonsterId();
    int getRandomObjId();

    void fillWithRandomObjects(int parentUID, int gen);




    void removeEntity(bool isReq, int ind);

    bool isRecycledFunc(int poolId);

    BaseObjType placeNewEnt(
        bool isReq,
        int et,
        FIVector4* cellPos,
        bool isHidden=false
    );

    void performDrag(
        bool isReq,

        int _draggingFromInd,
        int _draggingFromType,
        int _draggingToInd,
        int _draggingToType,

        FIVector4* _worldMarker
    );

    void setCurActor(int newUID);

    void toggleFirstPerson();

    void toggleActorSel();


    void setSelInd(int ind);

    void closeContainer(int i);

    void toggleCont(int contIndex, bool onMousePos);


    void addVisObject(BaseObjType _uid, bool isRecycled);

    bool removeVisObject(BaseObjType _uid, bool isRecycled);

    bool areEnemies(int actorUID1, int actorUID2);

    bool areFriends(int actorUID1, int actorUID2);

    int getUnitDisXY(btVector3 p1, btVector3 p2);

    btVector3 getUnitDistance(int actorUID1, int actorUID2);

    int getClosestActor(
        int actorId,
        int objType,
        float maxDis,
        uint flags
    );

    GameOrg* getCurOrg();

    BaseObj* getActorRef(int uid);

    bool combatMode();

    bool isSwingingWeapon(int actorId, int handNum);
    bool isPunching(int actorId, int handNum);
    bool isKicking(int actorId, int handNum);

    void setSwing(
        float _mx,
        float _my,
        int actorId,
        int handNum,
        bool isKick
    );

    void nextSwing(int actorId, int handNum);

    void makeShoot(int actorId, int bulletType);


    void bindPose(int actorId, int handNum, bool bindOn);



    void makeGrab(int actorId, int _handNum);


    void makeDropAll(int actorId);


    void makeThrow(int actorId, int _handNum);

    void changeStatus(int actorId, int status, int modVal);

    void makeSwing(int actorId, int handNum);

    void makeTurnTowardsTB(int actorId, btVector3 actorTargVec);

    BaseObj* getEntAtUnitPos(btVector3 pos);

    void makeTurnTB(int actorId, int modVal);

    btVector3 getOffsetTB(btVector3 orig, int dir, float amount);

    bool makeMoveTB(int actorId, int modVal);

    void makeTurn(int actorId, float dirFactor);

    void makeMoveVec(int actorId, btVector3 moveVec);

    void makeMove(int actorId, btVector3 moveDir, bool relative, bool delayed);

    void makeJump(int actorId, int isUp, float jumpFactor);

    void makeHit(
        bool tb,
        int attackerId,
        int victimId,
        int weaponId
    );


    GameOrgNode* getMirroredNode(GameOrgNode* curNode);

    void refreshActor(int actorId);

    void applyNodeChanges(GameOrgNode* _curNode, float dx, float dy);


    void transformOrg(
        GameOrg* curOrg,
        GameOrgNode* tempParent
    );




    void resetActiveNode();

    bool updateNearestOrgNode(bool setActive);


    void saveOrgFromMenu(string currentFieldString);

    void loadOrgFromMenu(string currentFieldString);


    void makeDirty();

    void setSelNode(GameOrgNode* newNode);


    bool hasRLBN(int rlbnRes, int k);

    void loadPoseInfo(bool justRefresh);


    GameOrg* getPose(
        int targPoseGroup,
        int targRLBN,
        int targStep
    );

    string getPoseString(
        int targPoseGroup,
        int targRLBN,
        int targStep
    );

    GameOrg* getCurrentPose();

    string getCurrentPoseString();


    int getActionStateFromPose(int poseNum);

    void changePose(int amount);


    void saveCurrentPose();


    void getIndexForPose(PoseKey* tempPose);

    void setPoseFromIndex(int i);

    int getPoseType(int poseIndex);


    void saveEveryPose();

    void loadNonPoseData(int npdPose, int npdSide, int npdStep);


    void loadCurrentPose();



    int numberIcons(int pCurCount, int x1, int y1, int x2, int y2);

    string getStringForObjectId(int objectId);

    void initAllObjects();

private:
    Singleton* singleton;

    bool curActorNeedsRefresh;

    bool orgOn;
    bool isDraggingObject;
    bool firstPerson;
    bool takingTurn;


    int weaponToPlace;
    int activeActorUID; // actor whose turn it is
    int curActorUID; // actor selected by human player
    int curPoseType;
    int highlightedLimb;
    int highlightedLimb2;
    int gameObjCounter;
    int lastObjInd;
    int selObjInd;
    int actObjInd;
    int draggingFromInd;
    int draggingToInd;
    int draggingFromType;
    int draggingToType;
    int turnListInd;



    float subjectDistance;
    float lastSubjectDistance;




    FIVector4 tempVec1;
    FIVector4 tempVec2;
    FIVector4 tempVec3;

    map<BaseObjType, BaseObj> gameObjects;
    vector<BaseObjType> visObjects;
    vector<int> turnList;


    GameOrgNode* bestNode;
    GameOrgNode* selectedNode;
    GameOrgNode* lastSelNode;
    GameOrgNode* activeNode;

    std::vector<GamePhysRig*> gamePhysRigs;
    std::vector<GameOrg*> gameOrgs;
    std::vector<GameOrg*> gamePoses;
    PoseInfo gamePoseInfo[E_PG_LENGTH];
    PoseKey curPose[E_ENTTYPE_LENGTH];
    EntPool entPoolStack[E_ENTTYPE_LENGTH];
    JSONValue *poseRootJS;

    int entIdToIcon[MAX_OBJ_TYPES];
    int iconToEntId[MAX_ICON_ID];
    bool isContainer[MAX_OBJ_TYPES];
    string objStrings[MAX_OBJ_TYPES];


    




};

#endif//_voxelquest__h_
