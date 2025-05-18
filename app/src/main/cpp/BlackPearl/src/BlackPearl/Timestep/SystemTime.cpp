//
// Created by DXT00 on 2025/5/14.
//
#include "pch.h"
#include "Timestep/SystemTime.h"

using namespace std::chrono;
namespace BlackPearl{
     std::chrono::milliseconds SystemTime::StartTimeMs;
     void SystemTime::Start(){
         StartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
     }

     milliseconds SystemTime::GetCurentTimeMs()
     {
         milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
         return currentTimeMs;
     }

     double SystemTime::GetRuntimeFromStartMs() {
         double runtime = GetCurentTimeMs().count() - StartTimeMs.count();
         return runtime;
     }
    
}