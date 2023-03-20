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

  Lidar = CreateDefaultSubobject<UVelodyneBaseComponent>(TEXT("VelodyneComponent"));
  this->AddOwnedComponent(Lidar);

  UdpScan = CreateDefaultSubobject<UUDPComponent>("UdpComponent");
  this->AddOwnedComponent(UdpScan);

  if(UdpScan)
  {
    ConfigureUDPScan();
  }
}

// Called when the game starts or when spawned
void ALidarBaseActor::BeginPlay()
{
  Super::BeginPlay();

  //FTimespan ThreadSleepTime = FTimespan::FromMilliseconds(1.367);
  FTimespan ThreadSleepTime =
  FTimespan::FromMilliseconds(Lidar->Sensor.NumberDataBlock * (Lidar->Sensor.NumberDataChannel / Lidar->Sensor.NumberLaserEmitter) * (0.000001f * FIRING_CYCLE));
  //Lidar->Sensor.NumberDataBlock * (Lidar->Sensor.NumberDataChannel / Lidar->Sensor.NumberLaserEmitter) * (0.000001f * FIRING_CYCLE); // nano sec -> milli sec
  FString UniqueThreadName = "LidarThread";

  LidarThread = new LidarThreadProcess(ThreadSleepTime,*UniqueThreadName, this);

  if(LidarThread)
  {
    LidarThread->Init();
    LidarThread->LidarThreadInit();
    UE_LOG(LogTemp, Warning, TEXT("Lidar thread initialized!"));
  }
}

void ALidarBaseActor::EndPlay(EEndPlayReason::Type Reason)
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

// Called every frame
// void ALidarBaseActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }

//! On Thread (not game thread)
//Never stop until finished calculating!
//This would be a verrrrry large hitch if done on game thread!
void ALidarBaseActor::LidarThreadTick()
{
  std::chrono::steady_clock::time_point Begin = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point End = std::chrono::steady_clock::now();

  //TArray<uint8> DataPacket;
  //DataPacket.AddUninitialized(120);
  uint32 TimeDiffMs = 0;
  uint32 ScanDataDiffMs = 0;
  uint32 DataPacketDiffMs = 0;
  uint32 EmitByteDiffMs = 0;

  //! Always check the Pointers
  //if(!MultiThreadDataCore)
  //{
  //  return;
  //}

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

  // Start raycasting
  Lidar->GetScanData();

  // Generate veldyne data packet to send UDP socket
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

  //Success!
  if(LidarThread)
  {
    //LidarThread->SetPaused(true);
  }
}

void ALidarBaseActor::ConfigureUDPScan()
{
  UdpScan->Settings.SendIP    = Lidar->DestinationIP;
  UdpScan->Settings.ReceiveIP = Lidar->SensorIP;
  UdpScan->Settings.SendPort  = Lidar->ScanPort;
  UdpScan->Settings.SendSocketName = FString(TEXT("ue5-scan-send"));
  UdpScan->Settings.BufferSize = PACKET_HEADER_SIZE + DATA_PACKET_PAYLOAD;
}