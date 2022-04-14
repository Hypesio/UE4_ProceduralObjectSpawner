// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduraleObjectSpawner.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProceduraleObjectSpawner::AProceduraleObjectSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProceduraleObjectSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduraleObjectSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Shot a ray from start to end and return touched position
FVector AProceduraleObjectSpawner::Raycast(FVector start, FVector end, FHitResult& outHit)
{
	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(this->GetOwner());

	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 2.5f, 0, 1);

	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionQueryParams);

	if (!isHit)
		return FVector::ZeroVector;

	return outHit.ImpactPoint;
}

bool AProceduraleObjectSpawner::SphereCast(FVector start, FVector end, float radius, AActor *actorToIgnore)
{
	FHitResult outHit;
	FCollisionShape shape = FCollisionShape();
	shape.SetSphere(radius);
	
	bool isHit = GetWorld()->SweepSingleByChannel(outHit, start, end, FQuat::Identity, ECC_Visibility, shape);
	return isHit;
}

void AProceduraleObjectSpawner::ShowSpawnRadius(float durationVisible, FLinearColor sphereColor, float thickness)
{
	FVector extent = FVector(1,1,1);
	extent.Normalize();
	UKismetSystemLibrary::DrawDebugBox(this, GetActorLocation(), spawnRadius * extent,  sphereColor, FRotator::ZeroRotator, durationVisible, thickness);
}

void AProceduraleObjectSpawner::SpawnObjects()
{
	if (onFloor && onCeiling)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("[ProceduraleObjectSpawner] Object can't be on floor and ceiling at the same time"));
		return;
	}
	
	FVector const spawnPosition = GetActorLocation();
	FRotator spawnRotation = GetActorRotation();
	UWorld* const world = GetWorld();

	//FVector refPos = spawnPosition + FVector::ForwardVector * spawnRadius * 2;
	// Create a reference to have mesh bounds data
	//AActor* actorReference = world->SpawnActor(blueprintToSpawn, &refPos, &spawnRotation);

	/*UStaticMeshComponent* staticMeshComp = actorReference->FindComponentByClass<UStaticMeshComponent>();
	UStaticMesh* staticMesh = staticMeshComp->GetStaticMesh();
	FBoxSphereBounds meshBounds;
	if (staticMesh != NULL)
		meshBounds = staticMesh->GetBounds();*/

	int nbSpawned = 0;
	int maxTry = numberToSpawn * 3;
	int nbTry = 0;
	while (nbSpawned < numberToSpawn && nbTry < maxTry)
	{
		nbTry ++;
		float posX, posY, posZ;
		
		posX = FMath::FRandRange(spawnPosition.X - spawnRadius, spawnPosition.X + spawnRadius);
		posY = FMath::FRandRange(spawnPosition.Y - spawnRadius, spawnPosition.Y + spawnRadius);
		posZ = FMath::FRandRange(spawnPosition.Z - spawnRadius, spawnPosition.Z + spawnRadius);

		FVector placeToSpawn = FVector(posX, posY, posZ);

		if (onFloor || onWall || onCeiling)
		{
			FVector start, end;
			if (onFloor)
			{
				start = FVector(posX, posY,GetActorLocation().Z);
				end = start + spawnRadius/2 * FVector::DownVector;
			}
			else if (onWall)
			{
				start = GetActorLocation();
				FVector direction = (placeToSpawn - start);
				direction.Normalize();
				end = start + direction * spawnRadius/2;
			}
			else if (onCeiling)
			{
				start = FVector(posX, GetActorLocation().Y,posZ);
				end = start + spawnRadius/2 * FVector::UpVector;
			}
			FHitResult outHit; 
			FVector pointHit = Raycast(start, end, outHit);
			placeToSpawn = pointHit;

			// We want objectDirection and Surface Normal to be colinear
			/*FVector surfaceNormal = outHit.ImpactNormal;
			FVector objectDirection = meshBounds.Origin;
			DrawDebugLine(GetWorld(), placeToSpawn, placeToSpawn + surfaceNormal * 10, FColor::Orange, false, 3.5f, 0, 1);
			DrawDebugLine(GetWorld(), placeToSpawn, placeToSpawn + objectDirection * 10, FColor::Blue, false, 3.5f, 0, 1);
			
			float angleRotation = FVector::DotProduct(surfaceNormal, objectDirection);
			angleRotation = UKismetMathLibrary::DegAcos(angleRotation);
			spawnRotation = surfaceNormal.RotateAngleAxis(angleRotation, FVector::UpVector).ToOrientationRotator();*/
		}

		if (spawnPlaceMustBeVisible)
		{
			// Is there something between spawn point and actor location ?
			FHitResult outHit;
			FVector objectInPath = Raycast(GetActorLocation(), placeToSpawn, outHit);
			if (objectInPath != placeToSpawn && objectInPath != FVector::ZeroVector)
			{
				continue;
			}
		}
		
		if (placeToSpawn != FVector::ZeroVector)
		{
			// Todo do a sphere cast to check if place is free before spawn
			world->SpawnActor(blueprintToSpawn, &placeToSpawn, &spawnRotation);
				
			nbSpawned ++;
		}
	}
	
	if (nbTry == maxTry)
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("[ProceduraleObjectSpawner] Can't spawn the amount of object wanted"));

	//actorReference->Destroy();
}