// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduraleMeshSpawner.h"

// Sets default values
AProceduraleMeshSpawner::AProceduraleMeshSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProceduraleMeshSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduraleMeshSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduraleMeshSpawner::SpawnObjects()
{
	FVector const spawnPosition = GetActorLocation();
	FRotator const spawnRotation = GetActorRotation();
	UWorld* const world = GetWorld();
	for (int i = 0; i < numberToSpawn; i++)
	{
		float randomX = FMath::FRandRange(spawnPosition.X - spawnRadius, spawnPosition.X + spawnRadius);
		float randomY = FMath::FRandRange(spawnPosition.Y - spawnRadius, spawnPosition.Y + spawnRadius);
		float randomZ = FMath::FRandRange(spawnPosition.Z - spawnRadius, spawnPosition.Z + spawnRadius);
		FVector placeToSpawn = FVector(randomX, randomY, randomZ);
		FActorSpawnParameters param;
		world->SpawnActor(blueprintToSpawn, &placeToSpawn, &spawnRotation);
	}
}