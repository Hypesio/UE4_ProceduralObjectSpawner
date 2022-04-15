// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduraleObjectSpawner.h"

#include "DrawDebugHelpers.h"
#include "EditorLevelUtils.h"
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
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), spawnRadius,  16, sphereColor, durationVisible, thickness);
}

void AProceduraleObjectSpawner::SpawnObjects()
{
	if (onFloor && onCeiling)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("[ProceduraleObjectSpawner] Object can't be on floor and ceiling at the same time"));
		return;
	}
	
	FVector const spawnerPosition = GetActorLocation();
	FRotator spawnRotation = GetActorRotation();
	UWorld* const world = GetWorld();

	// Create a reference to have mesh bounds data
	FVector refPos = spawnerPosition + FVector::ForwardVector * spawnRadius * 2;
	AActor* actorReference = world->SpawnActor(blueprintToSpawn, &refPos, &spawnRotation);

	UStaticMeshComponent* staticMeshComp = actorReference->FindComponentByClass<UStaticMeshComponent>();
	UStaticMesh* staticMesh = staticMeshComp->GetStaticMesh();
	FBoxSphereBounds meshBounds;
	if (staticMesh != NULL)
		meshBounds = staticMesh->GetBounds();

	int nbSpawned = 0;
	int maxTry = numberToSpawn * 3;
	int nbTry = 0;
	while (nbSpawned < numberToSpawn && nbTry < maxTry)
	{
		nbTry ++;
		float posX, posY, posZ;
		
		posX = FMath::FRandRange(spawnerPosition.X - spawnRadius, spawnerPosition.X + spawnRadius);
		posY = FMath::FRandRange(spawnerPosition.Y - spawnRadius, spawnerPosition.Y + spawnRadius);
		posZ = FMath::FRandRange(spawnerPosition.Z - spawnRadius, spawnerPosition.Z + spawnRadius);

		FVector placeToSpawn = FVector(posX, posY, posZ);

		if (onFloor || onWall || onCeiling)
		{
			
			FVector start, end;
			if (onFloor)
			{
				start = FVector(posX, posY,GetActorLocation().Z);
				float distance = FVector::Distance(spawnerPosition, start);
				end = start + (spawnRadius - distance) * FVector::DownVector;
			}
			else if (onWall)
			{
				start = GetActorLocation();
				FVector direction = (placeToSpawn - start);
				direction.Normalize();
				end = start + (direction * spawnRadius);
				// Todo check l'angle de la zone touché pour s'assure que c'est un mur range: 75° -> 105°
			}
			else if (onCeiling)
			{
				start = FVector(posX, GetActorLocation().Y,posZ);
				float distance = FVector::Distance(spawnerPosition, start);
				end = start + (spawnRadius - distance) * FVector::UpVector;
			}
			FHitResult outHit; 
			FVector pointHit = Raycast(start, end, outHit);
			placeToSpawn = pointHit;

			// We want objectDirection and Surface Normal to be collinear
			FVector surfaceNormal = outHit.ImpactNormal;
			FVector objectDirection = actorReference->GetActorForwardVector();
			
			DrawDebugLine(GetWorld(), placeToSpawn, placeToSpawn + (surfaceNormal * 45), FColor::Orange, false, 3.5f, 0, 1);
			DrawDebugLine(GetWorld(), placeToSpawn, placeToSpawn + (objectDirection * 45), FColor::Blue, false, 3.5f, 0, 1);
			
			float angleRotation = FVector::DotProduct(surfaceNormal, objectDirection);
			angleRotation = UKismetMathLibrary::DegAcos(angleRotation);
			if (onFloor || onCeiling)
				spawnRotation = surfaceNormal.RotateAngleAxis(angleRotation, FVector::UpVector).ToOrientationRotator();
			else
				spawnRotation = surfaceNormal.RotateAngleAxis(angleRotation, FVector::RightVector).ToOrientationRotator();
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
	
	actorReference->Destroy();
}