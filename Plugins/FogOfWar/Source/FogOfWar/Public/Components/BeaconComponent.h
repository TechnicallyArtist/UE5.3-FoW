// Copyright TechnicallyArtist

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BeaconComponent.generated.h"

class UTrackerComponent;
class AFogManager;

/**
 * Add to actors/characters which should serve
 * as beacons to player - giving them additional vision.
 */
UCLASS(Blueprintable)
class FOGOFWAR_API UBeaconComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UBeaconComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Character for which this component is beacon/visibility source. */
	void AssignTracker(UTrackerComponent* NewMasterTracker);

	UPROPERTY(EditAnywhere, Category = "Fog of War")
	float VisibilityRadius = 400.f;

private:
	UPROPERTY()
	UTrackerComponent* MasterTracker;

	UPROPERTY()
	TMap<AActor*, bool> OverlappingEnemyActors;

	UFUNCTION()
	void Server_OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void Server_CheckEnemyLineOfSightVisibility();

	UFUNCTION()
	void Server_OnActorEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
