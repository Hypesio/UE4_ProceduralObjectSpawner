// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduraleObjectSpawner.h"

#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
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

	if (showRaycast)
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 2.5f, 0, 1);

	bool isHit = GetWorld()->LineTraceSingleByChannel(outHit, start, end, ECC_Visibility, collisionQueryParams);

	if (!isHit)
		return FVector::ZeroVector;

	return outHit.ImpactPoint;
}

bool AProceduraleObjectSpawner::SphapeCast(FVector placeToCheck, const AActor* actorToSpawn, const AActor *actorToIgnore)
{
	TArray<FHitResult> outHits;
	FCollisionShape shape = FCollisionShape();
	shape.ShapeType = ECollisionShape::Capsule;
	actorToSpawn->GetSimpleCollisionCylinder(shape.Capsule.Radius, shape.Capsule.HalfHeight);
	
	const FQuat rotation = actorToSpawn->GetActorRotation().Quaternion();
	if (GetWorld()->SweepMultiByChannel(outHits, placeToCheck, placeToCheck, rotation, ECC_Visibility, shape))
	{
		for (auto& hit : outHits)
		{
			if (hit.GetActor() == actorToIgnore)
				continue;
			else 
				return true;
		}
	}
	

	return false;
}

void AProceduraleObjectSpawner::ShowSpawnRadius(float durationVisible, FLinearColor sphereColor, float thickness)
{
	FVector extent = FVector(1,1,1);
	extent.Normalize();
	UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation(), spawnRadius,  16, sphereColor, durationVisible, thickness);
}

FRotator RandomRotation(FRotator startRotation, bool randomX, bool randomY, bool randomZ)
{
	if (randomX)
	{
		startRotation.Roll = FMath::RandRange(0,360); 
	}
	if (randomY)
	{
		startRotation.Pitch = FMath::RandRange(0,360); 
	}
	if (randomZ)
	{
		startRotation.Yaw = FMath::RandRange(0,360); 
	}
	return startRotation;
}

// Return true if we can't spawn the object on the surface specified
bool IsSurfaceForbidden(const TArray<AActor*> forbiddenActors, const bool isMeshForbidden, const UStaticMesh* meshToSpawn,const AActor* surface)
{
	if (surface == NULL)
		return false;
	
	const UStaticMeshComponent* meshComp = surface->FindComponentByClass<UStaticMeshComponent>();
	if (meshComp == NULL)
		return false;
	const UStaticMesh* meshTouched = meshComp->GetStaticMesh();
	if (meshTouched == NULL)
		return false;
	
	if (meshTouched == meshToSpawn)
		return true;
	
	for (auto& actor : forbiddenActors)
	{
		if (isMeshForbidden)
		{
			const UStaticMeshComponent* meshCompForbidden = actor->FindComponentByClass<UStaticMeshComponent>();
			if (meshCompForbidden == nullptr)
				continue;
			const UStaticMesh* meshForbidden = meshComp->GetStaticMesh();
			if (meshTouched == nullptr)
				continue;
		
			if (meshForbidden == meshTouched)
				return true;
		}
		else
		{
			if (surface == actor)
				return true;
		}
	}
	
	return false;
}

// Check if point is at correct distance from previously spawned objects
bool IsPointAtGoodDistance(const TArray<AActor *> spawnedActor, FVector spawnPlace, float minDistance)
{
	if (minDistance <= 0.01f)
		return true;
	for (auto& actor : spawnedActor)
	{
		float dist = FVector::Distance(actor->GetActorLocation(), spawnPlace);
		if (dist < minDistance)
			return false;
	}
	return true;
}

// Spawn a blueprint or a mesh depending on parameters choosen
AActor* AProceduraleObjectSpawner::SpawnActorNeeded(const FVector spawnPosition, const FRotator spawnRotation)
{
	UWorld* const world = GetWorld();
	AActor* spawnedActor;
	if (typeToSpawn == Blueprint)
	{
		if (blueprintToSpawn == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("[ProceduraleObjectSpawner] No blueprint specified"));
			return nullptr;
		}
		spawnedActor = world->SpawnActor(blueprintToSpawn, &spawnPosition, &spawnRotation);
	}
	else
	{
		if (meshToSpawn == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("[ProceduraleObjectSpawner] No mesh specified"));
			return nullptr;
		}
		AStaticMeshActor* spawnedMesh = world->SpawnActor<AStaticMeshActor>();
		UStaticMeshComponent* meshComponent = spawnedMesh->GetStaticMeshComponent();
		meshComponent->SetStaticMesh(meshToSpawn);
		spawnedActor = spawnedMesh;
	}

	return spawnedActor; 
}

void AProceduraleObjectSpawner::SpawnObjects()
{
	FVector const spawnerPosition = GetActorLocation();
	FRotator startRotation = GetActorRotation();
	UWorld* const world = GetWorld();

	// Create a reference to have mesh bounds data
	FVector refPos = spawnerPosition + FVector::ForwardVector * spawnRadius * 2;
	
	AActor* actorReference = SpawnActorNeeded(refPos, startRotation);
	if (actorReference == nullptr)
		return;
	TArray<AActor *> actorsSpawned;

	UStaticMeshComponent* staticMeshComp = actorReference->FindComponentByClass<UStaticMeshComponent>();
	UStaticMesh* staticMesh = staticMeshComp->GetStaticMesh();
	FBoxSphereBounds meshBounds;
	if (staticMesh != NULL)
		meshBounds = staticMesh->GetBounds();

	int nbSpawned = 0;
	int maxTry = numberToSpawn * maximumTryMultiplier;
	int nbTry = 0;
	while (nbSpawned < numberToSpawn && nbTry < maxTry)
	{
		nbTry ++;
		float posX, posY, posZ;
		
		posX = FMath::FRandRange(spawnerPosition.X - spawnRadius, spawnerPosition.X + spawnRadius);
		posY = FMath::FRandRange(spawnerPosition.Y - spawnRadius, spawnerPosition.Y + spawnRadius);
		posZ = FMath::FRandRange(spawnerPosition.Z - spawnRadius, spawnerPosition.Z + spawnRadius);

		FVector placeToSpawn = FVector(posX, posY, posZ);
		AActor* surfaceTouched = NULL;

		FRotator spawnRotation = RandomRotation(startRotation, randomXRotation, randomYRotation, randomZRotation);

		FVector scale = actorReference->GetActorScale() * FMath::RandRange(scaleMin, scaleMax);
		actorReference->SetActorScale3D(scale);
		
		if (onFloor || onWall || onCeiling)
		{
			FVector start, end;
			// Use when obj can spawn on floor and ceiling
			bool callFloor = FMath::RandRange(0,1) == 0; 
			if (onWall && !onlyOnCorner)
			{
				start = GetActorLocation();
				FVector direction = (placeToSpawn - start);
				direction.Normalize();
				end = start + (direction * spawnRadius);
			}
			else if (onFloor && (!onCeiling || callFloor))
			{
				start = FVector(posX, posY,GetActorLocation().Z);
				float distance = FVector::Distance(spawnerPosition, start);
				end = start + (spawnRadius - distance) * FVector::DownVector;
			}
			else if (onCeiling && (!onFloor || !callFloor))
			{
				start = FVector(posX, GetActorLocation().Y,posZ);
				float distance = FVector::Distance(spawnerPosition, start);
				end = start + (spawnRadius - distance) * FVector::UpVector;
			}
			FHitResult outHit; 
			FVector pointHit = Raycast(start, end, outHit);
			placeToSpawn = pointHit;

			// We want objectDirection and Surface Normal to be collinear to correct object rotation
			FVector surfaceNormal = outHit.ImpactNormal;
			surfaceTouched = outHit.GetActor();
			FVector objectDirection = actorReference->GetActorForwardVector();
			
			
			// Check the surface normal angle to differentiate wall / floor / celling
			float angleSurface = FMath::RadiansToDegrees(acos(FVector::DotProduct(surfaceNormal, FVector::DownVector)));
			//UE_LOG(LogTemp, Warning, TEXT("Angle touched %f"), angleSurface);
			bool surfaceIsWall = angleSurface >= wallMinAngle && angleSurface <= wallMaxAngle;
			bool surfaceIsFloor = !surfaceIsWall && outHit.ImpactPoint.Z < spawnerPosition.Z;
			if ((!onFloor && !onCeiling && onWall) && !surfaceIsWall)
				continue;
			else if (!onWall && surfaceIsWall)
				continue;
			else if ((!onFloor && surfaceIsFloor) || (!onCeiling && !surfaceIsFloor && !surfaceIsWall))
				continue;

			// Search a corner if needed
			if (onlyOnCorner)
			{
				// Take a random vector rotating around the surface normal.
				// TODO
			}
			
			//Check if the surface touched is forbidden
			if (IsSurfaceForbidden(forbiddenList, typeForbidden == ForbiddenType::StaticMesh, staticMesh, surfaceTouched))
				continue;

			if (!IsPointAtGoodDistance(actorsSpawned, outHit.Location, distanceMinBetweenObjects))
				continue;

			if (showNormals)
			{
				DrawDebugLine(GetWorld(), placeToSpawn, placeToSpawn + (surfaceNormal * 45), FColor::Orange, false, 3.5f, 0, 1);
				DrawDebugLine(GetWorld(), placeToSpawn, placeToSpawn + (objectDirection * 45), FColor::Blue, false, 3.5f, 0, 1);
			}

			if (alignObjectWithSurface)
			{
				FRotator zxMatrix = UKismetMathLibrary::MakeRotFromZX(surfaceNormal,actorReference->GetActorForwardVector());
				zxMatrix.Yaw = 0;
				spawnRotation = zxMatrix;
				actorReference->SetActorRotation(spawnRotation);

				if (randomXRotation || randomYRotation || randomZRotation)
				{
					float angleRotate = FMath::RandRange(0.0f, 360.0f);
					FRotator localRotation = {angleRotate, 0 , 0};
					FVector rotatedVector = UKismetMathLibrary::RotateAngleAxis(actorReference->GetActorForwardVector(), angleRotate, actorReference->GetActorUpVector());
					FRotator rotation = UKismetMathLibrary::FindLookAtRotation(actorReference->GetActorForwardVector(), rotatedVector);
					actorReference->SetActorRotation(rotation);
				}
			}
		}

		if (spawnPlaceMustBeVisible)
		{
			// Is there something between spawn point and actor location ?
			FHitResult outHit;
			FVector objectInPath = Raycast(GetActorLocation(), placeToSpawn, outHit);
			if (objectInPath != placeToSpawn && objectInPath != FVector::ZeroVector && (surfaceTouched == NULL && outHit.GetActor() != surfaceTouched))
			{
				continue;
			}
		}
		
		if (placeToSpawn != FVector::ZeroVector)
		{
			if (!alignObjectWithSurface)
				actorReference->SetActorRotation(spawnRotation);
			
			if (SphapeCast(placeToSpawn, actorReference, surfaceTouched))
				continue;
			actorReference->SetActorLocation(placeToSpawn);
			actorsSpawned.Add(actorReference);
			
			nbSpawned ++;
			actorReference = SpawnActorNeeded(refPos, startRotation);
			staticMeshComp = actorReference->FindComponentByClass<UStaticMeshComponent>();
			staticMesh = staticMeshComp->GetStaticMesh();
		}
	}
	
	if (nbTry == maxTry)
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, TEXT("[ProceduraleObjectSpawner] Can't spawn the amount of object wanted"));
	
	actorReference->Destroy();
}