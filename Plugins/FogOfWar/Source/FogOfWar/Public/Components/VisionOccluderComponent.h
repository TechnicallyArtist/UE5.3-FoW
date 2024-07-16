// Copyright TechnicallyArtist

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VisionOccluderComponent.generated.h"

UCLASS( Blueprintable )
class FOGOFWAR_API UVisionOccluderComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void OnRegister() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Vision Occluder")
	void OnVisionOccluderConstruction();

};
