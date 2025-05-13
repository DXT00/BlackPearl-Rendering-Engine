//
// Created by DXT00 on 2025/5/12.
//

#ifndef BLACKPEARL_ANDROIDINPUTMANAGER_H
#define BLACKPEARL_ANDROIDINPUTMANAGER_H
#include "glm/glm.hpp"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "Core/CriticalSection.h"
#include "Core/ScopeLock.h"

#include <queue>
namespace BlackPearl{
    enum AndroidInputEvent
    {
        CameraRotationDelta,
        CameraPositionDelta,
    };

    struct FInputEventPacket
    {
        AndroidInputEvent event;

        int dx, dy;
    };



    class AndroidInputManager {
    public:
        static AndroidInputManager* GetInstance();

        void Tick(MainCamera* m_MainCamera, glm::vec3& cameraPos, CameraRotation& cameraRot);
        void EnqueueInputRotationEvent(int dx, int dy);
        void EnqueueInputPositionEvent(int dx, int dy);
        FInputEventPacket DequeueAppEvent();
    protected:
        AndroidInputManager();
        mutable FCriticalSection QueueMutex;

    private:
        static AndroidInputManager* sInstance;
       // pthread_mutex_t QueueMutex;
        std::queue<FInputEventPacket> Queue;


    };

}


#endif //BLACKPEARL_ANDROIDINPUTMANAGER_H
