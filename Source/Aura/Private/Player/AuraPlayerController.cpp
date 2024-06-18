// Copyright Elish Li


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"
#include  "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
    Super::BeginPlay();
    // it is an assertion to check if AuraContext doesn't get set, it will crash the game.
    check(AuraContext);
    // subsystem is a singleton to add mapping context
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    check(Subsystem);
    Subsystem->AddMappingContext(AuraContext, 0);
    
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
    UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
    //bind input actions, parameters are:
    //1st: a TObject of type <T>
    //2nd: Trigger event, when the callback will be triggered by player's controller
    //3rd: user object, in this case, it is this player controller, aka AuraPlayerController
    //4th: callback function that will be executed when triggered
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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
    FHitResult CursorHit;
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;
    // if there is a hit and the hit target has implemented IEnemyInterface, there will be a return to the CursorHit
    //otherwise, it will reture a null pointer in CursorHit
    Cast<IEnemyInterface>(CursorHit.GetActor());
    LastActor = ThisActor;
    ThisActor = CursorHit.GetActor();

    /*
     * Line trace from cursor, there are several senarios:
     * A. LastActor is null && ThisActor is null
     *  - Do nothing
     * B. LastActor is null && ThisActor is valid
     *  - Highlight ThisActor
     * C. LastActor is valid && ThisActor is null
     *  - Unhighlight LastActor
     * D. Both actors are valid, but LastActor != ThisActor
     *  - Unhighlight LastActor
     *  - Highlight ThisActor
     * E. Both actors are valid, and are the same actor
     *  - Do nothing
     */
    if (LastActor == nullptr)
    {
        if(ThisActor != nullptr)
        {
            //case B
            ThisActor->HighlightActor();
        }
        //case A
    }
    else
    {
        if (ThisActor == nullptr)
        {
            //case C
            LastActor->UnhighlightActor();
        }
        else
        {
            //both actors are valid
            if (LastActor != ThisActor)
            {
                //case D
                LastActor->UnhighlightActor();
                ThisActor->HighlightActor();
            }
            //case E
        }
    }
    
}

