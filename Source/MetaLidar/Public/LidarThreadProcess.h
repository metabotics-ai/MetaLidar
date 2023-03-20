// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LidarThreadBase.h"

class ALidarBaseActor;
/**
 *
 */
class METALIDAR_API LidarThreadProcess : public LidarThreadBase
{
public:
  typedef LidarThreadBase Super;

  LidarThreadProcess(const FTimespan& InThreadSleepTime, const TCHAR* InThreadName, ALidarBaseActor* InLidarActor)
  : Super(InThreadSleepTime, InThreadName), LidarActor(InLidarActor)
  {}

  virtual void Process() override;

  bool LidarThreadInit();
  void LidarThreadShutdown();

protected:

  //! Because non protected ptr, (not UPROPERTY())
  //! >>> The owning actor of this thread must outlive the thread itself <<<
  //! Owning actor stops this thread on EndPlay, its own destructor starting process
  //! Owning actor freezes game thread until this thread acknowledges it has been stopped
  ALidarBaseActor* LidarActor = nullptr;
};