// Copyright Elish Li


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"
#include  "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
    bReplicates = true;
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

