//
// Created by DXT00 on 2025/5/12.
//
#include "pch.h"
#include "Core.h"
#include "Luanch/Android/AndroidInputManager.h"

#include "Application.h"

namespace BlackPearl
{
    AndroidInputManager* AndroidInputManager::sInstance = NULL;
    AndroidInputManager* AndroidInputManager::GetInstance()
    {
        if(!sInstance)
        {
            sInstance = new AndroidInputManager();
        }

        return sInstance;
    }
    void AndroidInputManager::EnqueueInputRotationEvent(int dx, int dy){
       
        FInputEventPacket Event;
        Event.event = CameraRotationDelta;
        Event.dx = dx; //yaw
        Event.dy = dy; //pitch
        FScopeLock Lock(&QueueMutex);

        Queue.push(Event);

       // int rc = pthread_mutex_lock(&QueueMutex);
       // GE_ASSERT(rc == 0);
      


       // rc = pthread_mutex_unlock(&QueueMutex);
       // GE_ASSERT(rc == 0);

    }

    void AndroidInputManager::EnqueueInputPositionEvent(int dx, int dy){
        FInputEventPacket Event;
        Event.event = CameraPositionDelta;
        Event.dx = dx; //x
        Event.dy = dy; //y
        FScopeLock Lock(&QueueMutex);

        Queue.push(Event);


        /*int rc = pthread_mutex_lock(&QueueMutex);
        GE_ASSERT(rc == 0);
     

        rc = pthread_mutex_unlock(&QueueMutex);
        GE_ASSERT(rc == 0);*/

    }
    AndroidInputManager::AndroidInputManager(){
      //  pthread_mutex_init(&QueueMutex, NULL);

    }
    FInputEventPacket AndroidInputManager::DequeueAppEvent()
    {
        FScopeLock Lock(&QueueMutex);

       // int rc = pthread_mutex_lock(&QueueMutex);
        //GE_ASSERT(rc == 0);

        FInputEventPacket OutData;
        OutData = Queue.front();
        Queue.pop();

       // rc = pthread_mutex_unlock(&QueueMutex);
        //GE_ASSERT(rc == 0);

        //UE_LOG(LogAndroidEvents, Display, ("LogAndroidEvents::DequeueAppEvent : %u, [width=%d, height=%d], %s"), OutData.State, OutData.Data.WindowWidth, OutData.Data.WindowHeight, GetAppEventName(OutData.State))

        return OutData;
    }


    void AndroidInputManager::Tick(MainCamera* m_MainCamera, glm::vec3 &cameraPos, CameraRotation &cameraRot)
    {
        // GE_ASSERT(IsInGameThread());
        while (!Queue.empty())
        {
            FInputEventPacket Event = DequeueAppEvent();
            // FPlatformMisc::LowLevelOutputDebugStringf(("FAppEventManager::Tick processing, %d"), int(Event.State));

            switch (Event.event)
            {
                case CameraRotationDelta:
                {
                    math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();

                    float degreesPerPixelX = (m_MainCamera->Fov() / (float)windowSize.x);// *camera.data.aspect;
                    float degreesPerPixelY = (m_MainCamera->Fov() / (float)windowSize.y);


                    float maxRotDelta = 2 * m_MainCamera->GetRotateSpeed();
                    float deltaX = Event.dx;
                    float deltaY = Event.dy;

                     deltaX = deltaX * degreesPerPixelX ;//ts *
                     deltaY = deltaY * degreesPerPixelY ;//ts *

                    if (deltaX > maxRotDelta)
                        deltaX = maxRotDelta;
                    if (deltaY > maxRotDelta)
                        deltaY = maxRotDelta;


                    cameraRot.Yaw += deltaX * m_MainCamera->GetRotateSpeed();
                    cameraRot.Pitch += deltaY * m_MainCamera->GetRotateSpeed() ;

                    if (cameraRot.Pitch > 89.0f)
                        cameraRot.Pitch = 89.0f;
                    if (cameraRot.Pitch < -89.0f)
                        cameraRot.Pitch = -89.0f;

                    if (cameraRot.Yaw > 0.0f)
                        cameraRot.Yaw = 0.0f;
                    if (cameraRot.Yaw < -360.0f)
                        cameraRot.Yaw = -360.0f;



                    GE_CORE_WARN("[dxt00] main camera set rot, yaw:%f, pitch: %f, speed=%f", cameraRot.Pitch, cameraRot.Yaw, m_MainCamera->GetRotateSpeed());
                    m_MainCamera->SetRotation({ cameraRot.Pitch, cameraRot.Yaw, 0.0f });
                }
                    break;
                case CameraPositionDelta:
                {

                    float maxMoveDelta = 2 * m_MainCamera->GetMoveSpeed();
                    float deltaX = Event.dx;
                    float deltaY = Event.dy;
                    if (deltaX > maxMoveDelta)
                        deltaX = maxMoveDelta;
                    if (deltaY > maxMoveDelta)
                        deltaY = maxMoveDelta;


                    cameraPos += m_MainCamera->Right() * (deltaX > 0 ? m_MainCamera->GetMoveSpeed() : -m_MainCamera->GetMoveSpeed());


                    cameraPos += m_MainCamera->Front() * (deltaY > 0 ? -m_MainCamera->GetMoveSpeed() : m_MainCamera->GetMoveSpeed());
                    GE_CORE_WARN("[dxt00] main camera set pos, x:%f, y:%f, z:%f,  speed=%f", cameraPos.x, cameraPos.y, cameraPos.z, m_MainCamera->GetMoveSpeed());
                    m_MainCamera->SetPosition(cameraPos);
                }
                    break;

                default:
                    break;
                    //  UE_LOG(LogAndroidEvents, Display, ("Application Event : %u  not handled. "), Event.State);
            }


        }


    }


}