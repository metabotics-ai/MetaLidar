// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/GenericPlatformTime.h"
#include "LidarThreadProcess.h"
THIRD_PARTY_INCLUDES_START
#include <UDPComponent.h>
THIRD_PARTY_INCLUDES_END
#include "LidarBaseActor.generated.h"

UCLASS()
class METALIDAR_API ALidarBaseActor : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ALidarBaseActor();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MetaLidar")
  UStaticMeshComponent* LidarMeshComponent;

  class UUDPComponent* UdpScanComponent;

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  // Called when the game end
  virtual void EndPlay(EEndPlayReason::Type Reason) override;

  uint32 PacketTimestamp  = 0;
  double BeginTimestamp   = 0.0;

public:
  /**
  * Set UDP communication parameters for scan data
  */
  virtual void ConfigureUDPScan(){}

  /**
  * Main routine
  * calculate raytracing and generate LiDAR packet data
  */
  virtual void LidarThreadTick(){}

  class LidarThreadProcess* LidarThread = nullptr;

};
