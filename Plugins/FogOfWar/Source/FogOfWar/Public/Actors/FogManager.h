// Copyright TechnicallyArtist

#pragma once

#include "CoreMinimal.h"

#include "FogManager.generated.h"

class UTrackerComponent;
class UNiagaraComponent;
class UBeaconComponent;
class UPostProcessComponent;

/**
 * - Updates postprocess with the location of locally controlled character.
 * - Holds general fog settings.
 * - Important! Local shadows are baked to a Render Target, when testing from editor,
 * this means that both clients will write to same RT_Shadows target, and one client will update both player's shadows.
 * To test shadows, run game as two processes, by going to Play -> Advanced Settings -> Disabling Run Under One Process.
 */
UCLASS()
class FOGOFWAR_API AFogManager : public AActor
{
	GENERATED_BODY()

public:
	AFogManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Used to get all actors which should be excluded from casting shadows. */
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetAllNonOccludingActors() const;
public:
	FORCEINLINE float GetMapSize() const { return MapSize; }
	FORCEINLINE float GetCharacterVisibilityRadius() const { return CharacterVisibilityRadius; }

	void UpdateCharacterLocation();

#pragma region Settings
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fog of War")
	bool bIsMapCentered = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fog of War")
	float MapSize = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Fog of War")
	float CharacterVisibilityRadius = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
	UMaterialInterface* FogMaterial;

	UPROPERTY(EditAnywhere, Category = "Fog of War", meta = (ToolTip = "By default, fog material uses baker's render target. To improve performance it is recommended to bake a texture and set it here."))
	UTexture2D* FloorMapTexture;
#pragma endregion

protected:
	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* FoWMaterialInstance;

	UPROPERTY(BlueprintReadOnly)
	UTrackerComponent* LocalTracker;

private:
	UPROPERTY()
	UPostProcessComponent* FogPostProcessComponent;

	UFUNCTION(BlueprintCallable)
	UTrackerComponent* GetLocalTracker();

#pragma region Beacon locations
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FVector4> BeaconLocationsAndSizes;
private:
	void UpdateBeaconLocations();
#pragma endregion
};
