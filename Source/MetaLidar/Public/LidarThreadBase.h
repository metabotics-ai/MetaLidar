/*
* This is based on Rama's UE Multi-threading
*/
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"

#include "Misc/SingleThreadRunnable.h"
/**
 *
 */
class METALIDAR_API LidarThreadBase : public FRunnable, FSingleThreadRunnable
{
public:
  /**
  * @brief create the thread by calling this
  * @param InThreadSleepTime The sleep time in main loop of thread.
  * @param InThreadName The thread description (for debugging).
  **/
  LidarThreadBase(const FTimespan& InThreadSleepTime, const TCHAR* InThreadName)
  : bStopping(false), ThreadSleepTime(InThreadSleepTime)
  {
    Paused.AtomicSet(false);
    HasStopped.AtomicSet(true);

    Thread = FRunnableThread::Create(this, InThreadName, 0U, EThreadPriority::TPri_TimeCritical, FPlatformAffinity::GetPoolThreadMask());
    if (Thread == nullptr)
    {
      UE_LOG(LogTemp, Error, TEXT("Thread has not been created in Constructor"))
    }
  }

  virtual ~LidarThreadBase()
  {
    if (Thread != nullptr)
    {
      Thread->Kill(true);
      delete Thread;
      Thread = nullptr;

      UE_LOG(LogTemp, Warning, TEXT("Thread has been deleted in Destructor"));
    }
  }

public:
  // FSingleThreadRunnable interface

  // Returns a pointer to the single threaded interface when mulithreading is disabled.
  virtual FSingleThreadRunnable* GetSingleThreadInterface() override
  {
    return this;
  }

  // FSingleThreadRunnable interface
  virtual void Tick() override
  {
    Process();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~
  //To be Subclassed
  virtual void Process() {}
  //~~~~~~~~~~~~~~~~~~~~~~~

public:
  // FRunnable interface
  // Do not call these functions youself, that will happen automatically

  // Do your setup here, allocate memory, ect.
  virtual bool Init() override
  {
    return true;
  }

  // Main data processing happens here
  virtual uint32 Run() override
  {
    HasStopped.AtomicSet(false);

    while (!bStopping)
    {
      if(Paused)
      {
        if(!IsVerifiedSuspended)
        {
          IsVerifiedSuspended.AtomicSet(true);
        }

        //! Ready to resume on a moments notice though!
        FPlatformProcess::Sleep(ThreadSleepTime.GetTotalSeconds());

        continue;
      }

      Process();
    }

    HasStopped.AtomicSet(true);
    return 0;
  }

  // Clean up any memory you allocated here
  virtual void Stop() override
  {
    SetPaused(true);
    bStopping = true;
  }

public:

  void SetPaused(bool MakePaused)
  {
    Paused.AtomicSet(MakePaused);
    if(!MakePaused)
    {
      IsVerifiedSuspended.AtomicSet(false);
    }
  }

  bool IsThreadPaused()
  {
    return Paused;
  }

  bool IsThreadVerifiedSuspended()
  {
    return IsVerifiedSuspended;
  }

  bool ThreadHasStopped()
  {
    return HasStopped;
  }

protected:

  FThreadSafeBool Paused;
  FThreadSafeBool IsVerifiedSuspended;
  FThreadSafeBool HasStopped;

  // Thread object. Control the thread using this, with operators like Kill and Suspend
  FRunnableThread* Thread = nullptr;

  // Holds a flag indicating that the thread is stopping.
  bool bStopping;

public:
  // Holds the amount of time to wait
  FTimespan ThreadSleepTime;
};

