// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "MyCharacter.generated.h"


UCLASS()


class TEST_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
		//meta = (AllowprivateAccess = 'true')
public:
	// Sets default values for this character's properties
	AMyCharacter();
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Player")
		class UStaticMeshComponent* Cube;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly,Category="Spawn")
	TSubclassOf<AActor> ActorToSpawn;

	UFUNCTION()
		void SpawnObject(FVector Loc,FRotator Rot);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float value);
	void MoveRight(float value);
	void TurnAtRate(float value);
	void LookUpRate(float value);
	void Jump(float value);
	void InteractPressed();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
		float TraceDistance;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	


	UFUNCTION( BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	UPROPERTY(EditAnywhere, Category = "TimeLine")
		UCurveFloat* CurveFloat;

	//
	UPROPERTY(BlueprintReadWrite, Category = "Fire")
		float ImpulseForce;
	void FireForward();

	UPROPERTY(BlueprintReadWrite, Category = "Fire")
		bool bApplyRadialForce;
	UPROPERTY(BlueprintReadWrite, Category = "Fire")
		float RadialImpactForce;
	UPROPERTY(BlueprintReadWrite, Category = "Fire")
		float Impactradius;

	TArray<FHitResult> HitActors;

	void FireModeChange();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override;

private:
	AActor* FocusedActor;

	FVector StartScale;
	FVector TargetScale;
	FTimeline SquashTimeline;

	UFUNCTION()
	void SquashProgress(float value);
};
