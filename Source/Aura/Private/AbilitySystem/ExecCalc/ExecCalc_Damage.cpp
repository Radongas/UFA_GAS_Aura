// Copyright Elish Li


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	//a macro creating Armor_Property and Armor_Def of FGameplayEffectAttributeCaptureDefinition
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceFire)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceLightning)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceArcane)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistancePhysical)

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
	AuraDamageStatics()
	{
		//The MMCs are examples of manually setting up the Attribute Capture Definitions
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistanceFire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistanceLightning, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistanceArcane, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ResistancePhysical, Target, false);

		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Fire, ResistanceFireDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Lightning, ResistanceLightningDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Arcane, ResistanceArcaneDef);
		TagsToCaptureDefs.Add(FAuraGameplayTags::Get().Attributes_Resistance_Physical, ResistancePhysicalDef);
	}
};

//this static function make sure we always get the same DamageStatics containing the Armor definition
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceFireDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceLightningDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceArcaneDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistancePhysicalDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//do not have to call Super
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Damage = 0.f;
	//loop through all the damage types involving in this GE and calculate the ultimate damage
	for (const auto& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
		checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
		
		DamageTypeValue *= (100.f - Resistance)/100.f;
		
		Damage += DamageTypeValue;
	}
	

	//Capture Block Chance on Target, and determine if there was a successful block
	//If block, halve the damage
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(TargetBlockChance, 0.f);
	bool IsBlock = false;
	if (TargetBlockChance >= FMath::RandRange(1,100))
	{
		IsBlock = true;
		Damage /= 2.f;
	}

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, IsBlock);
	
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, SourceArmorPenetration);
	TargetArmor = FMath::Max(SourceArmorPenetration, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve =  CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient =  ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	
	const FRealCurve* EffectiveArmorCurve =  CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient =  EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	
	
	//Armor penetration ignore a percentage of the target's armor
	const float EffectArmor = TargetArmor *= FMath::Max(0.f, (100 - SourceArmorPenetration * ArmorPenetrationCoefficient)) / 100.f;
	Damage *= (100 - EffectArmor * EffectiveArmorCoefficient) / 100.f;


	//critical hit resistance critical hit chance by a certain percentage
	//critical hit only applies when the attack is not hit
	if (!IsBlock)
	{
		float TargetCriticalHitResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
		TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance, 0.f);
		
		float SourceCriticalHitChance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
		SourceCriticalHitChance = FMath::Max(SourceCriticalHitChance, 0.f);
		
		float SourceCriticalHitDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max(SourceCriticalHitDamage, 0.f);

		const FRealCurve* CriticalHitResistanceCoefficientCurve = CharacterClassInfo->CriticalHitResistanceCoefficient->FindCurve(FName("CriticalHitResistanceCoefficient"), FString());
		const float CriticalHitResistanceCoefficient = CriticalHitResistanceCoefficientCurve->Eval(TargetCombatInterface->GetPlayerLevel());

		//if the critical hit occurs, the damage will become: damage * 2 + CriticalHitDamage
		if (FMath::Max(SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient, 0.f) >= FMath::RandRange(1,100))
		{
			UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, true);
			Damage *= 2;
			Damage += SourceCriticalHitDamage;
		}
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
