// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduraleObjectSpawner.generated.h"

UCLASS()
class PFE_3IS_API AProceduraleObjectSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduraleObjectSpawner();
    UPROPERTY(EditAnywhere, Category = MainParameters)
    UClass* blueprintToSpawn;
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
	bool alignObjectWithSurface = true;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool randomXRotation;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool randomYRotation;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool randomZRotation;
	
	/** Warning: size grow really fast stay under 2 to avoid crash */
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
    float scaleMax = 1;
    UPROPERTY(EditAnywhere, Category = AdvancedParameters)
    float scaleMin = 1;

	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	float wallMinAngle = 75;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	float wallMaxAngle = 115;
	
	/** True => spawn on mesh of same type as actor in list will be forbidden
	 * False => spawn on actors specified will be forbidden */
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	bool isMeshForbidden;
	UPROPERTY(EditAnywhere, Category = AdvancedParameters)
	TArray<AActor*> forbiddenSpawn;

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
};