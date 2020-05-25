// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Components/TimelineComponent.h"
#include "InteractInterface.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//RootComponent = Cube;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm -> SetupAttachment(RootComponent);
	//SpringArm->TargetArmLength = 400;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera -> SetupAttachment(SpringArm);
	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Cube->SetupAttachment(RootComponent);
	
	//Cube->SetSimulatePhysics(true);
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	TraceDistance = 2000.0f;

	ImpulseForce = 500.0f;

	bApplyRadialForce = true;
	RadialImpactForce = 2000.0f;
	Impactradius = 200.0f;

	StartScale = FVector(1, 1, 1);
	TargetScale = FVector(1.3f, 1.3f, 0.8f);

	//Cube->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapBegin);
}

void AMyCharacter::SpawnObject(FVector Loc,FRotator Rot)
{
	FActorSpawnParameters SpawnParams;
	AActor* SpwanActorRef = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Loc, Rot, SpawnParams);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	Cube->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnOverlapBegin);
	
}

void AMyCharacter::MoveForward(float value)
{
	if ((Controller) && (value != 0.0f))
	{
		const FRotator Rotation = Controller-> GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);

	}
}

void AMyCharacter::MoveRight(float value)
{
	if ((Controller) && (value != 0.0f))
	{
		const FRotator Rotation = Controller-> GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);

	}
}

//void AMyCharacter::TurnAtRate(float value)
//{
//	AddControllerYawInput(value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
//
//}
//
//void AMyCharacter::LookUpRate(float value)
//{
//	AddControllerPitchInput(value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
//}


void AMyCharacter::InteractPressed()
{
	TraceForward();

	if (FocusedActor)
	{
		IInteractInterface* Interface = Cast<IInteractInterface>(FocusedActor);
		if (Interface)
		{
			Interface->Execute_OnInteract(FocusedActor,this);
		}
	}
}

void AMyCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		IInteractInterface* Interface = Cast<IInteractInterface>(OtherActor);
		if (Interface)
		{
			Interface->Execute_OnInteract(OtherActor, this);
		}
	}
}

void AMyCharacter::TraceForward_Implementation()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	GetController()->GetPlayerViewPoint(Loc, Rot);
	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);
	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);
	if (bHit)
	{
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(5, 5, 5), FColor::Green, false, 3.0f);
		AActor* Interactable = Hit.GetActor();

		if (Interactable)
		{
			if (Interactable != FocusedActor)
			{
				if (FocusedActor)
				{
					IInteractInterface* Interface = Cast<IInteractInterface>(FocusedActor);
					if (Interface)
					{
						Interface->Execute_EndFocus(FocusedActor);
					}
				}
				IInteractInterface* Interface = Cast<IInteractInterface>(Interactable);
				if (Interface)
				{
					Interface->Execute_StartFocus(Interactable);
				}
				FocusedActor = Interactable;
			}
		}
		else
		{
			if (FocusedActor)
			{
				IInteractInterface* Interface = Cast<IInteractInterface>(FocusedActor);
				if (Interface)
				{
					Interface->Execute_EndFocus(FocusedActor);
				}
			}

			FocusedActor = nullptr;
		}
	}
}

void AMyCharacter::FireForward()
{
	FVector Loc;
	FRotator Rot;
	FHitResult Hit;
	GetController()->GetPlayerViewPoint(Loc, Rot);
	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * TraceDistance);
	FCollisionQueryParams TraceParams;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

	if (bHit)
	{
		SpawnObject(Hit.Location, Rot);
		if (bApplyRadialForce)
		{


			FCollisionShape SphereCol = FCollisionShape::MakeSphere(Impactradius);

			bool bSweepHit = GetWorld()->SweepMultiByChannel(HitActors, Hit.Location, Hit.Location + FVector(0.01f, 0.01f, 0.01f),
				FQuat::Identity, ECC_WorldStatic, SphereCol);
			DrawDebugSphere(GetWorld(), Hit.Location, Impactradius, 50, FColor::Orange, false, 2.0f);
			if (bSweepHit)
			{
				for (auto& Hit : HitActors)
				{
					UStaticMeshComponent* Mesh1 = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
					if (Mesh1)
					{
						Mesh1->AddRadialImpulse(Hit.Location, Impactradius, RadialImpactForce, ERadialImpulseFalloff::RIF_Constant, true);
					}
				}
			}
		}
		else if (Hit.GetActor()->IsRootComponentMovable())
		{
			UStaticMeshComponent* Mesh1 = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
			if (Mesh1)
			{
				FVector CameraForward = Camera->GetForwardVector();
				Mesh1->AddImpulse(CameraForward * ImpulseForce * Mesh1->GetMass());
			}
			
		}
	}
}

void AMyCharacter::FireModeChange()
{
	if (bApplyRadialForce)
	{
		bApplyRadialForce = false;
	}
	else
		bApplyRadialForce = true;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);
	TraceForward();
	
	
	SquashTimeline.TickTimeline(DeltaTime);
	

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &AMyCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AMyCharacter::LookUpRate);
	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::InteractPressed);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyCharacter::FireForward);
	PlayerInputComponent->BindAction("FireMode", IE_Pressed, this, &AMyCharacter::FireModeChange);


}

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CurveFloat)
	{
		FOnTimelineFloat TimeLineProgress;
		TimeLineProgress.BindUFunction(this, FName("SquashProgress"));
		SquashTimeline.AddInterpFloat(CurveFloat, TimeLineProgress);
		SquashTimeline.SetLooping(false);

	

		SquashTimeline.PlayFromStart();
	}
}

void AMyCharacter::SquashProgress(float value)
{
	FVector NewScale = FMath::Lerp(StartScale, TargetScale, value);
	Cube->SetWorldScale3D(NewScale);
}

