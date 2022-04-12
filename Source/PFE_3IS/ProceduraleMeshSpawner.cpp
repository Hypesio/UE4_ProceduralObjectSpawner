// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduraleMeshSpawner.h"

#include <Actor.h>

#include "DrawDebugHelpers.h"

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

// Shot a ray from start to end and return touched position
FVector AProceduraleMeshSpawner::Raycast(FVector start, FVector end)
{
	FHitResult outHit;
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(this->GetOwner());

	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 1, 0, 1);

	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionQueryParams);

	if (!isHit)
		return FVector::ZeroVector;

	return outHit.ImpactPoint;
}

void AProceduraleMeshSpawner::SpawnObjects()
{
	FVector const spawnPosition = GetActorLocation();
	FRotator const spawnRotation = GetActorRotation();
	UWorld* const world = GetWorld();

	UStaticMeshComponent* staticMeshComp = ((AActor*)blueprintToSpawn->GetClass())->FindComponentByClass<UStaticMeshComponent>();
	UStaticMesh* staticMesh = staticMeshComp->GetStaticMesh();
	for (int i = 0; i < numberToSpawn; i++)
	{
		float posX, posY, posZ;
		
		posX = FMath::FRandRange(spawnPosition.X - spawnRadius, spawnPosition.X + spawnRadius);
		posY = FMath::FRandRange(spawnPosition.Y - spawnRadius, spawnPosition.Y + spawnRadius);
		posZ = FMath::FRandRange(spawnPosition.Z - spawnRadius, spawnPosition.Z + spawnRadius);

		FVector placeToSpawn = FVector(posX, posY, posZ);
		if (onFloor)
		{
			FVector start = FVector(posX, GetActorLocation().Y,posZ);
			FVector end = start + spawnRadius * FVector::DownVector;
			FVector pointHit = Raycast(start, end);

			placeToSpawn = pointHit;
		}
		
		FActorSpawnParameters param;
		world->SpawnActor(blueprintToSpawn, &placeToSpawn, &spawnRotation);
	}
}