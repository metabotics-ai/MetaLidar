#include "LidarThreadProcess.h"
#include "LidarBaseActor.h"

void LidarThreadProcess::Process()
{
  if(!LidarActor)
  {
    return;
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // This is the connection from a wrapper of OS Threading to UE game code.
  // The thread tick should NOT call any UE code that
  //  1. Creates or destroys objects
  //  2. Modifies the game world in any way
  //  3. Tries to debug draw anything
  //  4. Simple raw data calculations are best!
  LidarActor->LidarThreadTick();
}

bool LidarThreadProcess::LidarThreadInit()
{
  //Any third party C++ to do on init
  return true;
}

void LidarThreadProcess::LidarThreadShutdown()
{
  //Any third party C++ to do on shutdown
}