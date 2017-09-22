#ifndef LIVING_LIFE_PAGE_INCLUDED
#define LIVING_LIFE_PAGE_INCLUDED




#include "minorGems/ui/event/ActionListener.h"
#include "minorGems/util/SimpleVector.h"

#include "minorGems/game/game.h"


#include "transitionBank.h"

#include "GamePage.h"


#include "pathFind.h"

#include "animationBank.h"

#include "TextField.h"


#define NUM_HUNGER_BOX_SPRITES 20

#define NUM_TEMP_ARROWS 6
#define NUM_HUNGER_DASHES 6

#define NUM_HINT_SHEETS 4


typedef struct LiveObject {
        int id;

        int displayID;
        char onScreen;
        
        double age;
        double ageRate;
        
        double lastAgeSetTime;
        
        int foodStore;
        int foodCapacity;
        
        int maxFoodStore;
        int maxFoodCapacity;

        // -1 unless we're currently being held
        // by an adult
        int heldByAdultID;
        
        // usually 0, unless we're being held by an adult
        // and just got put down
        // then we slide back into position
        doublePair heldByDropOffset;

        // the actual world pos we were last held at
        doublePair lastHeldByRawPos;
        
        
        // usually 0, but used to slide into and out of riding position
        doublePair ridingOffset;
        

        // 0 or positive holdingID means holding nothing or an object
        // a negative number here means we're holding another player (baby)
        // and the number, made positive, is the ID of the other player
        int holdingID;
        int lastHoldingID;

        char holdingFlip;

        char heldPosOverride;
        char heldPosOverrideAlmostOver;
        doublePair heldObjectPos;
        double heldObjectRot;
        int heldPosSlideStepCount;
        
        AnimType curAnim;
        AnimType lastAnim;
        double lastAnimFade;

        // anim tracking for held object
        AnimType curHeldAnim;
        AnimType lastHeldAnim;
        double lastHeldAnimFade;



        // furture states that curAnim should fade to, one at a time
        SimpleVector<AnimType> *futureAnimStack;
        SimpleVector<AnimType> *futureHeldAnimStack;
        
        // store frame counts in fractional form
        // this allows animations that can vary in speed without
        // ever experiencing discontinuities 
        // (an integer frame count, with a speed modifier applied later
        // could jump backwards in time when the modifier changes)
        double animationFrameCount;
        double heldAnimationFrameCount;

        double lastAnimationFrameCount;
        double lastHeldAnimationFrameCount;
        

        double frozenRotFrameCount;
        double heldFrozenRotFrameCount;
        
        char frozenRotFrameCountUsed;
        char heldFrozenRotFrameCountUsed;
        

        float heat;
        

        int numContained;
        int *containedIDs;
        SimpleVector<int> *subContainedIDs;
        
        ClothingSet clothing;
        // stacks of items contained in each piece of clothing
        SimpleVector<int> clothingContained[ NUM_CLOTHING_PIECES ];

        // current fractional grid position and speed
        doublePair currentPos;
        // current speed is move delta per frame
        double currentSpeed;

        // for instant reaction to move command when server hasn't
        // responded yet
        // in grid spaces per sec
        double lastSpeed;

        // recompute speed periodically during move so that we don't
        // fall behind when frame rate fluctuates
        double timeOfLastSpeedUpdate;
        
        // destination grid position
        int xd;
        int yd;
        
        // last confirmed stationary position of this
        // object on the server (from the last player_update)
        int xServer;
        int yServer;
        

        int pathLength;
        GridPos *pathToDest;

                
        int closestDestIfPathFailedX;
        int closestDestIfPathFailedY;
        

        int currentPathStep;
        doublePair currentMoveDirection;
        
        int numFramesOnCurrentStep;

        char onFinalPathStep;


        // how long whole move should take
        double moveTotalTime;
        
        // wall clock time in seconds object should arrive
        double moveEtaTime;

        
        char inMotion;
        
        char displayChar;
        
        int actionTargetX;
        int actionTargetY;
        
        // tweak for when we are performing an action on a moving object
        // that hasn't reach its destination yet.  actionTargetX,Y is the
        // destination, but this is the closest cell where it was at
        // when we clicked on it.
        int actionTargetTweakX;
        int actionTargetTweakY;

        char pendingAction;
        float pendingActionAnimationProgress;

        
        // NULL if no active speech
        char *currentSpeech;
        double speechFade;
        // wall clock time when speech should start fading
        double speechFadeETATime;


        char shouldDrawPathMarks;
        double pathMarkFade;
        

        // messages that arrive while we're still showing our current
        // movement animation
        SimpleVector<char*> pendingReceivedMessages;
        char somePendingMessageIsMoreMovement;

    } LiveObject;









typedef struct PointerHitRecord {
        int closestCellX;
        int closestCellY;
        int hitSlotIndex;

        char hit;
        char hitSelf;
    
        char hitOtherPerson;
        
        int hitClothingIndex;

        // when we click in a square, only count as hitting something
        // if we actually clicked the object there.  Else, we can walk
        // there if unblocked.
        char hitAnObject;

        // for case where we hit an object that we remembered placing
        // which may be behind
        // should NEVER click through a person
        char hitOurPlacement;

        // true if hitOurPlacement happened THROUGH another non-person object
        char hitOurPlacementBehind;
        

    } PointerHitRecord;



typedef struct OldArrow {
        int i;
        float heat;
        float fade;
    } OldArrow;



// for objects moving in-transit in special cases where we can't store
// them in the map (if they're moving onto an occupied space that should
// only change when they're done moving)
// We track them separately from the map until they are done moving
typedef struct ExtraMapObject {
        int objectID;

        double moveSpeed;
        doublePair moveOffset;
        
        int animationFrameCount;
        int animationLastFrameCount;
        
        int animationFrozenRotFrameCount;
        
        AnimType curAnimType;
        AnimType lastAnimType;
        double lastAnimFade;
        char flip;

        SimpleVector<int> containedStack;
        SimpleVector< SimpleVector<int> > subContainedStack;
    } ExtraMapObject;
        
        



class LivingLifePage : public GamePage {
        
    public:

        LivingLifePage();
        ~LivingLifePage();
        
        void clearMap();


        char isMapBeingPulled();

        // destroyed by caller
        // can be NULL
        char *getDeathReason();

        // prevent a jitter when frameRateFactor changes due to fps lag
        void adjustAllFrameCounts( double inOldFrameRateFactor,
                                   double inNewFrameRateFactor );

        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );
        
        virtual void step();
  
        virtual void makeActive( char inFresh );
        

        virtual void pointerMove( float inX, float inY );
        virtual void pointerDown( float inX, float inY );
        virtual void pointerDrag( float inX, float inY );
        virtual void pointerUp( float inX, float inY );

        virtual void keyDown( unsigned char inASCII );
        virtual void specialKeyDown( int inKeyCode );
        
        virtual void keyUp( unsigned char inASCII );

        
    protected:

        int mServerSocket;

        int mFirstServerMessagesReceived;
        
        char mStartedLoadingFirstObjectSet;
        char mDoneLoadingFirstObjectSet;

        float mFirstObjectSetLoadingProgress;
        
        

        // an offset that we apply to all server-recieved coordinates
        // before storing them locally, and reverse-apply to all local
        // coordinates before sending them to the server.

        // This keeps our local coordinates in a low range and prevents
        // rounding errors caused by rendering huge integers as 32-bit floats
        // on most graphics cards.
        //
        // We base this on the center of the first map chunk received
        char mMapGlobalOffsetSet;
        GridPos mMapGlobalOffset;

        // conversion function for received coordinates into local coords
        void applyReceiveOffset( int *inX, int *inY );
        // converts local coors for sending back to server
        int sendX( int inX );
        int sendY( int inY );


        int mMapD;

        int *mMap;
        
        int *mMapBiomes;
        int *mMapFloors;

        char *mMapCellDrawnFlags;

        int *mMapAnimationFrameCount;
        int *mMapAnimationLastFrameCount;
        
        int *mMapAnimationFrozenRotFrameCount;
        

        int *mMapFloorAnimationFrameCount;


        // all tiles on ground work their way toward animation type of
        // "ground" but may have a lingering types after being dropped
        AnimType *mMapCurAnimType;
        AnimType *mMapLastAnimType;
        double *mMapLastAnimFade;
        
        // 0,0 for most, except for a newly-dropped object
        // as it slides back into grid position
        doublePair *mMapDropOffsets;
        double *mMapDropRot;

        SoundUsage *mMapDropSounds;
        

        // 0, 0 for most, except objects that are moving
        doublePair *mMapMoveOffsets;
        // speed in CELL_D per sec
        double *mMapMoveSpeeds;
        

        // true if left-right flipped (to match last drop)
        // not tracked on server, so resets when object goes off of screen
        char *mMapTileFlips;
        

        SimpleVector<int> *mMapContainedStacks;

        SimpleVector< SimpleVector<int> > *mMapSubContainedStacks;
        
        
        // true if this map spot was something that our
        // player was responsible for placing
        char *mMapPlayerPlacedFlags;
        
        SimpleVector<GridPos> mMapExtraMovingObjectsDestWorldPos;
        SimpleVector<int> mMapExtraMovingObjectsDestObjectIDs;
        SimpleVector<ExtraMapObject> mMapExtraMovingObjects;

        
        int mMapOffsetX;
        int mMapOffsetY;


        char mEKeyDown;
        

        SpriteHandle mGuiPanelSprite;
        

        SpriteHandle mHungerBoxSprites[ NUM_HUNGER_BOX_SPRITES ];
        SpriteHandle mHungerBoxFillSprites[ NUM_HUNGER_BOX_SPRITES ];

        SpriteHandle mHungerBoxErasedSprites[ NUM_HUNGER_BOX_SPRITES ];
        SpriteHandle mHungerBoxFillErasedSprites[ NUM_HUNGER_BOX_SPRITES ];
        
        SpriteHandle mTempArrowSprites[ NUM_TEMP_ARROWS ];
        SpriteHandle mTempArrowErasedSprites[ NUM_TEMP_ARROWS ];

        SpriteHandle mHungerDashSprites[ NUM_HUNGER_DASHES ];
        SpriteHandle mHungerDashErasedSprites[ NUM_HUNGER_DASHES ];
        SpriteHandle mHungerBarSprites[ NUM_HUNGER_DASHES ];
        SpriteHandle mHungerBarErasedSprites[ NUM_HUNGER_DASHES ];

        SpriteHandle mNotePaperSprite;

        // offset from current view center
        doublePair mNotePaperHideOffset;
        doublePair mNotePaperPosOffset;
        doublePair mNotePaperPosTargetOffset;
        
        SimpleVector<char*> mLastKnownNoteLines;
        
        SimpleVector<char> mErasedNoteChars;
        SimpleVector<doublePair> mErasedNoteCharOffsets;
        SimpleVector<float> mErasedNoteCharFades;

        SimpleVector<char> mCurrentNoteChars;
        SimpleVector<doublePair> mCurrentNoteCharOffsets;
        
        SimpleVector<char*> mSentChatPhrases;

        
        SoundSpriteHandle mHungerSound;
        char mPulseHungerSound;
        
        SpriteHandle mHungerSlipSprites[3];

        // offset from current view center
        doublePair mHungerSlipHideOffsets[3];
        doublePair mHungerSlipShowOffsets[3];
        doublePair mHungerSlipPosOffset[3];
        doublePair mHungerSlipPosTargetOffset[3];
        
        double mHungerSlipWiggleTime[3];
        double mHungerSlipWiggleAmp[3];
        double mHungerSlipWiggleSpeed[3];
        
        // for catching dir change at peak of starving slip
        // to time sound
        double mStarvingSlipLastPos[2];
                                    

        // index of visble one, or -1
        int mHungerSlipVisible;
        
        
        SpriteHandle mHintSheetSprites[NUM_HINT_SHEETS];
        
        // offset from current view center
        doublePair mHintHideOffset[NUM_HINT_SHEETS];
        doublePair mHintPosOffset[NUM_HINT_SHEETS];
        doublePair mHintTargetOffset[NUM_HINT_SHEETS];
        
        doublePair mHintExtraOffset[NUM_HINT_SHEETS];

        // # separates lines
        char *mHintMessage[NUM_HINT_SHEETS];
        int mHintMessageIndex[NUM_HINT_SHEETS];
        int mNumTotalHints[ NUM_HINT_SHEETS ];

        int mLiveHintSheetIndex;

        int mCurrentHintObjectID;
        int mCurrentHintIndex;
        
        int mNextHintObjectID;
        int mNextHintIndex;

        SimpleVector<TransRecord *> mLastHintSortedList;
        int mLastHintSortedSourceID;
        
        // table sized to number of possible objects
        int *mHintBookmarks;
        

        int getNumHints( int inObjectID );
        char *getHintMessage( int inObjectID, int inIndex );


        // -1 if outside bounds of locally stored map
        int getMapIndex( int inWorldX, int inWorldY );
        

        int mCurrentArrowI;
        float mCurrentArrowHeat;
        
        SimpleVector<OldArrow> mOldArrows;
        
        char *mCurrentDes;
        SimpleVector<char*> mOldDesStrings;
        SimpleVector<float> mOldDesFades;
        

        char *mCurrentLastAteString;
        int mCurrentLastAteFillMax;
        SimpleVector<char*> mOldLastAteStrings;
        SimpleVector<int> mOldLastAteFillMax;
        SimpleVector<float> mOldLastAteFades;
        SimpleVector<float> mOldLastAteBarFades;

        
        void drawHungerMaxFillLine( doublePair inAteWordsPos,
                                    int inMaxFill,
                                    SpriteHandle *inBarSprites,
                                    SpriteHandle *inDashSprites,
                                    char inSkipBar,
                                    char inSkipDashes );
        

        int mLastMouseOverID;
        int mCurMouseOverID;
        float mCurMouseOverFade;
        
        GridPos mCurMouseOverSpot;
        char mCurMouseOverBehind;
        

        SimpleVector<GridPos> mPrevMouseOverSpots;
        SimpleVector<char> mPrevMouseOverSpotsBehind;
        SimpleVector<float> mPrevMouseOverSpotFades;
        

        float mLastMouseOverFade;

        SpriteHandle mChalkBlotSprite;
        SpriteHandle mPathMarkSprite;
        SpriteHandle mGroundOverlaySprite[4];
        
        
        // not visible, but used for its text filtering
        // capabilities
        TextField mSayField;
        
        double mPageStartTime;

        void computePathToDest( LiveObject *inObject );


        LiveObject *getOurLiveObject();
        LiveObject *getLiveObject( int inID );
        

        void clearLiveObjects();
        
        void drawChalkBackgroundString( doublePair inPos, 
                                        const char *inString,
                                        double inFade,
                                        double inMaxWidth,
                                        int inForceMinChalkBlots = -1 );
        

        // returns an animation pack that can be used to draw the
        // held object.  The pack's object ID is -1 if nothing is held
        ObjectAnimPack drawLiveObject( 
            LiveObject *inObj,
            SimpleVector<LiveObject *> *inSpeakers,
            SimpleVector<doublePair> *inSpeakersPos );
        

        void drawMapCell( int inMapI, 
                          int inScreenX, int inScreenY,
                          char inHighlightOnly = false );
        
        void checkForPointerHit( PointerHitRecord *inRecord,
                                 float inX, float inY );
        


        void handleOurDeath();
        

        char *mDeathReason;
        


        // handles error detection, total byte counting, etc.
        void sendToServerSocket( char *inMessage );
        

        ExtraMapObject copyFromMap( int inMapI );
        
        void putInMap( int inMapI, ExtraMapObject *inObj );
        

        char getCellBlocksWalking( int inMapX, int inMapY );
        

    };



#endif
