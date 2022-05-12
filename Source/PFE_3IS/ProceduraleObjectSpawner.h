// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduraleObjectSpawner.generated.h"

UENUM()
enum SpawnType
{
	Mesh,
	Blueprint
};

UENUM()
enum ForbiddenType
{
	StaticMesh,
	Actor
};

UCLASS()
class PFE_3IS_API AProceduraleObjectSpawner : public AActor
{
	GENERATED_BODY()
	
	
public:
	// Sets default values for this actor's properties
	AProceduraleObjectSpawner();

	UPROPERTY(EditAnywhere, Category = MainParameters)
	TEnumAsByte<SpawnType> typeToSpawn;
    UPROPERTY(EditAnywhere, Category = MainParameters, meta = (EditCondition = "typeToSpawn == SpawnType::Blueprint"))
    UClass* blueprintToSpawn;
	UPROPERTY(EditAnywhere, Category = MainParameters, meta = (EditCondition = "typeToSpawn == SpawnType::Mesh"))
	UStaticMesh* meshToSpawn;
	
    UPROPERTY(EditAnywhere, Category = MainParameters)
    int numberToSpawn;
    UPROPERTY(EditAnywhere, Category = MainParameters)
    float spawnRadius;
    UPROPERTY(EditAnywhere, Category = MainParameters)
    bool onFloor;
	UPROPERTY(EditAnywhere, Category = MainParameters)
	bool onCeiling;
	UPROPERTY(EditAnywhere, Category = MainParameters)
	bool onWall;
	/** Check if spawn place is visible from spawner place*/
	UPROPERTY(EditAnywhere, Category = MainParameters)
	bool spawnPlaceMustBeVisible = true;

	/** Align the object with surface touched. Limit random rotation*/
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool alignObjectWithSurface = false;
	/** Objects will be at te junction between wall and floor (or ceiling) depending on parameters chosen*/
	//UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool onlyOnCorner = false;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool randomXRotation;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters, meta = (EditCondition = "!alignObjectWithSurface"))
	bool randomYRotation;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters, meta = (EditCondition = "!alignObjectWithSurface"))
	bool randomZRotation;
	
	/** Warning: size grow really fast stay under 2 to avoid crash */
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
    float scaleMax = 1;
    UPROPERTY(EditAnywhere, Category = AdvancedParameters)
    float scaleMin = 1;

	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	float distanceMinBetweenObjects = 0;
	
	/** StaticMesh => spawn on mesh of same type as actor in list will be forbidden
	 * Actor => spawn on actors specified will be forbidden */
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	TEnumAsByte<ForbiddenType> typeForbidden = ForbiddenType::StaticMesh;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	TArray<AActor*> forbiddenList;

	UPROPERTY(EditAnywhere, Category = AdvancedParameters, meta = (ClampMin = "0", ClampMax = "180"))
	float wallMinAngle = 75;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters, meta = (ClampMin = "0", ClampMax = "180"))
	float wallMaxAngle = 115;
	/** Tell how many positions will be tested : maximumTryMultiplier * numberToSpawn*/
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	int maximumTryMultiplier = 50;
	
	UPROPERTY(EditAnywhere, Category = Debug)
	bool showRaycast = false;
	UPROPERTY(EditAnywhere, Category = Debug)
	bool showNormals = false;
	
    	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category="Spawn")
	void SpawnObjects();
	UFUNCTION(BlueprintCallable, Category="Debug")
	void ShowSpawnRadius(float durationVisible, FLinearColor sphereColor, float thickness);
	
private:
    FVector Raycast(FVector start, FVector end, FHitResult& outHit);
	bool SphapeCast(FVector placeToCheck, const AActor* actorToSpawn, const AActor *actorToIgnore);
	AActor* SpawnActorNeeded(const FVector spawnPosition, const FRotator spawnRotation);
};