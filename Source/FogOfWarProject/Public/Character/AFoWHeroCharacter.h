// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/FoWBaseCharacter.h"
#include "AFoWHeroCharacter.generated.h"

class UTrackerComponent;
class AFogManager;
enum class ETeam : uint8;
class UTrackedComponent;

/**
 * An example character using Fog of War plugin.
 * We create simple team organization for each character.
 * While your teams will likely be different, you will still need to call TrackerComponent's AssignEnemy to add enemy actors for tracking.
 */
UCLASS(Blueprintable)
class AFoWHeroCharacter : public AFoWBaseCharacter
{
	GENERATED_BODY()

public:
	AFoWHeroCharacter();

#pragma region Topdown (not fog of war related, but used to control topdown camera)

public:
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	void MoveCamera(const FVector2D& CameraMovementVector) const;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	FVector InitialCameraForward;
	FVector InitialCameraRight;

	void InitCustomCamera();
#pragma endregion
};

