// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduraleMeshSpawner.generated.h"

UCLASS()
class PFE_3IS_API AProceduraleMeshSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduraleMeshSpawner();
	UPROPERTY(EditAnywhere, Category = Parameters)
	UClass* blueprintToSpawn;
	UPROPERTY(EditAnywhere, Category = Parameters)
	int numberToSpawn;
	UPROPERTY(EditAnywhere, Category = Parameters)
	float spawnRadius;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Spawn")
	void SpawnObjects();
};