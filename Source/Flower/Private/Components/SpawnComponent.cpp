#include "Components/SpawnComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Character.h"

USpawnComponent::USpawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAbsoluteScale = true;
	bAbsoluteRotation = false;
	ArrowColor = FColor::Red;
	ArrowSize = 0.5f;
}

void USpawnComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Despawn();

	Super::EndPlay(EndPlayReason);
}

void USpawnComponent::Spawn(FFlowerSpawnParams& SpawnParams)
{
	if (SpawnParams.ActorClass->IsChildOf(APawn::StaticClass()))
	{
		UBehaviorTree* LoadedBT = LoadAsset<UBehaviorTree>(SpawnParams.BehaviorTree);
		SpawnedActor = UAIBlueprintHelperLibrary::SpawnAIFromClass(this, SpawnParams.ActorClass.Get(), LoadedBT, GetComponentLocation(), GetComponentRotation(), SpawnParams.bNoCollisionFail);

		if (SpawnedActor.IsValid() && (SpawnParams.AnimInstance || SpawnParams.AnimationAsset))
		{
			if (const ACharacter* Character = Cast<ACharacter>(SpawnedActor))
			{
				SetAnimationOnSpawnedMesh(Character->GetMesh(), SpawnParams);
			}
			else if (const APawn* Pawn = Cast<APawn>(SpawnedActor))
			{
				if (USkeletalMeshComponent* SkeletalMesh = Pawn->FindComponentByClass<USkeletalMeshComponent>())
				{
					SetAnimationOnSpawnedMesh(SkeletalMesh, SpawnParams);
				}
			}
		}
	}
	else
	{
		SpawnedActor = GetWorld()->SpawnActor(SpawnParams.ActorClass, &GetComponentTransform());
	}

	if (SpawnedActor.IsValid() && SpawnParams.ActorScale != 1.0f)
	{
		SpawnedActor->SetActorScale3D(SpawnedActor->GetActorScale3D() * SpawnParams.ActorScale);
	}
}

void USpawnComponent::Despawn()
{
	if (SpawnedActor.IsValid())
	{
		SpawnedActor->Destroy();
	}

	SpawnedActor = nullptr;
}

void USpawnComponent::SetAnimationOnSpawnedMesh(USkeletalMeshComponent* SkeletalMeshComponent, const FFlowerSpawnParams& SpawnParams) const
{
	if (SpawnParams.AnimInstance)
	{
		SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SkeletalMeshComponent->SetAnimInstanceClass(SpawnParams.AnimInstance);
	}
	else if (SpawnParams.AnimationAsset)
	{
		SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		SkeletalMeshComponent->SetAnimation(SpawnParams.AnimationAsset);
	}
}
