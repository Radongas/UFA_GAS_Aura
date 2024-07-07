// Copyright Elish Li


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacter.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;

    Spline = CreateDefaultSubobject<USplineComponent>("Spline");

    
    
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CursorTrace();

    AutoRun();
}

void AAuraPlayerController::AutoRun()
{
    if (!bAutoRunning) return;
    if(APawn* ControlledPawn = GetPawn())
    {
        // this vector is the location on the spline that's closest to the controlled pawn
        const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
        const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

        ControlledPawn->AddMovementInput(Direction);
        const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
        if (DistanceToDestination <= AutoRunAcceptanceRadius)
        {
            bAutoRunning = false;
        }
    }
}


void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();
    // it is an assertion to check if AuraContext doesn't get set, it will crash the game.
    check(AuraContext);
    // subsystem is a singleton to add mapping context
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    //this will work for multiplayer because the local machine will have the subsystem while the replication from other clients wont have
    if (Subsystem)
    {
        Subsystem->AddMappingContext(AuraContext, 0);
    }
    
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    
    //input mode struct containing mode data
    FInputModeGameAndUI InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
    
}



void AAuraPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // a cast check from InputComponent to UEnhancedInputComponent, fail will crash the game
    UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
    //bind input actions, parameters are:
    //1st: a TObject of type <T>
    //2nd: Trigger event, when the callback will be triggered by player's controller
    //3rd: user object, in this case, it is this player controller, aka AuraPlayerController
    //4th: callback function that will be executed when triggered
    AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
    AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
    AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
    const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0.f, Rotation.Yaw, 0.f   );
    
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    if (APawn* ControlledPawn = GetPawn<APawn>())
    {
        ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
        ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
    }
}

void AAuraPlayerController::CursorTrace()
{
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;
    // if there is a hit and the hit target has implemented IEnemyInterface, there will be a return to the CursorHit
    //otherwise, it will return a null pointer in CursorHit
    Cast<IEnemyInterface>(CursorHit.GetActor());
    LastActor = ThisActor;
    ThisActor = CursorHit.GetActor();

    if (LastActor != ThisActor)
    {
        if (LastActor)
        {
            LastActor->UnhighlightActor();
        }
        if (ThisActor)
        {
            ThisActor->HighlightActor();
        }
    }
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        bTargeting = ThisActor ? true : false;
        bAutoRunning = false;
    }
}
    

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetAsc())
        {
            GetAsc()->AbilityInputTagReleased(InputTag);
        }
        return;
    }

    if (GetAsc())
    {
        GetAsc()->AbilityInputTagReleased(InputTag);
    }
    
    if (!bTargeting && !bShiftKeyDown)
    {
        const APawn* ControlledPawn = GetPawn();
        if (FollowTime <= ShortPressThreshold)
        {
            if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
            {
                Spline->ClearSplinePoints();
                for (const FVector& PointLoc : NavPath->PathPoints)
                {
                    Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
                    //DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
                }
                CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
                bAutoRunning = true;
            }
        }
        FollowTime = 0.f;
        bTargeting = false;
    }
    
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
    {
        if (GetAsc())
        {
            GetAsc()->AbilityInputTagHeld(InputTag);
        }
        return;
    }

    if (bTargeting || bShiftKeyDown)
    {
        if (GetAsc())
        {
            GetAsc()->AbilityInputTagHeld(InputTag);
        }
    }
    else
    {
        //we are not activating ability, we are trying to move the character
        FollowTime += GetWorld()->GetDeltaSeconds();
        
        if (CursorHit.bBlockingHit)
        {
            CachedDestination = CursorHit.ImpactPoint;
        }

        if (APawn* ControlledPawn = GetPawn())
        {
            const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            ControlledPawn->AddMovementInput(WorldDirection);
        }
        
    }
}



UAuraAbilitySystemComponent* AAuraPlayerController::GetAsc()
{
    if (AuraAbilitySystemComponent == nullptr)
    {
        AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    }
    return AuraAbilitySystemComponent;
}

