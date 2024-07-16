// Copyright TechnicallyArtist

#pragma once

#include "CoreMinimal.h"
#include "TeamInterface.h"
#include "GameFramework/Character.h"
#include "FoWBaseCharacter.generated.h"


struct FTeamData;

UCLASS()
class FOGOFWARPROJECT_API AFoWBaseCharacter : public ACharacter, public ITeamInterface
{
	GENERATED_BODY()

public:
	AFoWBaseCharacter();

protected:
	virtual void BeginPlay() override;

#pragma region Teams
	/**
	 * Example team organization in multiplayer, your project will likely be different.
	 */
public:
	/** For player controlled character's this is called when match starts to assign teams automatically. */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SetTeam(const int32 InTeamNumber);

	virtual FTeamData GetTeamInfo_Implementation() const override { return TeamInfo; }
protected:
	/**
	 * This can probably be kept on another class, such as player state.
	 * For the sake of simplicity, we will set team colors here.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Fog of War")
	TArray<FTeamData> Teams;

	UPROPERTY(Replicated, EditAnywhere, Category = "Fog Of War")
	FTeamData TeamInfo;
private:
	void SetTeamColor();

	/** SetTeamColors will create an instance and set glow color. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> BodyMaterialClass;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BodyDynamicMaterial;

#pragma endregion

};
