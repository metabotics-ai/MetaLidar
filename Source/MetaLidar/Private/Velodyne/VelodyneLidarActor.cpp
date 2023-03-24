// Fill out your copyright notice in the Description page of Project Settings.

#include "Velodyne/VelodyneLidarActor.h"

// Sets default values
AVelodyneLidarActor::AVelodyneLidarActor()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = false;

  Lidar = CreateDefaultSubobject<UVelodyneBaseComponent>(TEXT("VelodyneComponent"));
  this->AddOwnedComponent(Lidar);
}

// Called when the game starts or when spawned
void AVelodyneLidarActor::BeginPlay()
{
  Super::BeginPlay();

  if(UdpScan)
  {
    ConfigureUDPScan();
    UdpScan->OpenSendSocket(UdpScan->Settings.SendIP, UdpScan->Settings.SendPort);
    UdpScan->OpenReceiveSocket(UdpScan->Settings.ReceiveIP, UdpScan->Settings.SendPort);
  }

  FTimespan ThreadSleepTime =
  FTimespan::FromMilliseconds(Lidar->Sensor.NumberDataBlock * (Lidar->Sensor.NumberDataChannel / Lidar->Sensor.NumberLaserEmitter) * (0.000001f * FIRING_CYCLE));
  FString UniqueThreadName = "LidarThread";

  LidarThread = new LidarThreadProcess(ThreadSleepTime,*UniqueThreadName, this);

  if(LidarThread)
  {
    LidarThread->Init();
    LidarThread->LidarThreadInit();
    UE_LOG(LogTemp, Warning, TEXT("Lidar thread initialized!"));
  }
}

void AVelodyneLidarActor::EndPlay(EEndPlayReason::Type Reason)
{
  if(LidarThread)
  {
    LidarThread->LidarThreadShutdown();
    LidarThread->Stop();
  }

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //! Wait here until WorkerThread is verified as having stopped!
  //!
  //! This is a safety feature, that will delay PIE EndPlay or
  //! closing of the game while complex calcs occurring on the
  //! thread have a chance to finish
  //!
  while(!LidarThread->ThreadHasStopped())
  {
    FPlatformProcess::Sleep(0.1);
  }
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  //Do this last
  delete LidarThread;

  Super::EndPlay(Reason);
}

// ! On Thread (not game thread)
// Never stop until finished calculating!
// This would be a verrrrry large hitch if done on game thread!
void AVelodyneLidarActor::LidarThreadTick()
{
  std::chrono::steady_clock::time_point Begin = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point End = std::chrono::steady_clock::now();

  uint32 TimeDiffMs = 0;

  //! Make sure to come all the way out of all function routines with this same check
  //! so as to ensure thread exits as quickly as possible, allowing game thread to finish
  //! See EndPlay for more information.
  if(LidarThread && LidarThread->IsThreadPaused())
  {
    return;
  }

  EndTimestamp = std::chrono::steady_clock::now();
  PacketTimestamp += (uint32)(std::chrono::duration_cast<std::chrono::microseconds>(EndTimestamp - BeginTimestamp).count());
  BeginTimestamp = std::chrono::steady_clock::now();

  Begin = std::chrono::steady_clock::now();

  // Generate raycasting data
  Lidar->GetScanData();

  // Generate veldyne data packet
  Lidar->GenerateDataPacket(PacketTimestamp);

  // Multicast Delegate event for broadcating packet data
  UdpScan->EmitBytes(Lidar->Sensor.DataPacket);

  End = std::chrono::steady_clock::now();

  TimeDiffMs = (uint32)(LidarThread->ThreadSleepTime.GetTotalMicroseconds() -
    std::chrono::duration_cast<std::chrono::microseconds>(End - Begin).count());

  if( TimeDiffMs > 0 )
  {
    FPlatformProcess::SleepNoStats(0.000001f * TimeDiffMs);
  }
  else
  {
    FPlatformProcess::SleepNoStats(0);
  }
  //UE_LOG(LogTemp, Warning, TEXT("Processing time: %f ms, Time diff : %u Ms"), std::chrono::duration_cast<std::chrono::microseconds>(End - Begin).count()*0.001f, TimeDiffMs);
}

void AVelodyneLidarActor::ConfigureUDPScan()
{
  UdpScan->Settings.SendIP    = Lidar->DestinationIP;
  UdpScan->Settings.ReceiveIP = Lidar->SensorIP;
  UdpScan->Settings.SendPort  = Lidar->ScanPort;
  UdpScan->Settings.SendSocketName = FString(TEXT("ue5-scan-send"));
  UdpScan->Settings.BufferSize = PACKET_HEADER_SIZE + DATA_PACKET_PAYLOAD;
}