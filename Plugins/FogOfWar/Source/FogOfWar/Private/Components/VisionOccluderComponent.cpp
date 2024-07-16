// Copyright TechnicallyArtist


#include "Components/VisionOccluderComponent.h"


void UVisionOccluderComponent::OnRegister()
{
	Super::OnRegister();

	OnVisionOccluderConstruction();
}


