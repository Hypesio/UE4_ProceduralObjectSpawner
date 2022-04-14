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
    UPROPERTY(EditAnywhere, Category = Parameters)
    UClass* blueprintToSpawn;
    UPROPERTY(EditAnywhere, Category = Parameters)
    int numberToSpawn;
    UPROPERTY(EditAnywhere, Category = Parameters)
    float spawnRadius;
    UPROPERTY(EditAnywhere, Category = Parameters)
    bool onFloor;
	UPROPERTY(EditAnywhere, Category = Parameters)
	bool onCeiling;
	UPROPERTY(EditAnywhere, Category = Parameters)
	bool onWall;
	UPROPERTY(EditAnywhere, Category = Parameters)
	bool spawnPlaceMustBeVisible;
    	
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
	bool SphereCast(FVector start, FVector end, float radius, AActor *actorToIgnore);
};