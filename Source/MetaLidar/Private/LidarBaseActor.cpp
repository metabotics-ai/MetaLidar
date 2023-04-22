// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarBaseActor.h"


// Sets default values
ALidarBaseActor::ALidarBaseActor() : PacketTimestamp(0)
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = false;

  // Our root component will be a static mesh
  LidarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
  RootComponent = LidarMeshComponent;

  UdpScanComponent = CreateDefaultSubobject<UUDPComponent>("UdpComponent");
  this->AddOwnedComponent(UdpScanComponent);
}

// Called when the game starts or when spawned
void ALidarBaseActor::BeginPlay()
{
  Super::BeginPlay();
}

void ALidarBaseActor::EndPlay(EEndPlayReason::Type Reason)
{
  Super::EndPlay(Reason);
}