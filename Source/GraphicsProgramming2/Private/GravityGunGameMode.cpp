// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityGunGameMode.h"
#include "Logging/StructuredLog.h"

void AGravityGunGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// 1. Print to Screen
	// Display a debug message for five seconds. Its color is red.
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello, world! This is GravityGunGameMode! A game that will be developed for both Graphics Programming 2 and Project 2 of Physics for Game Programmers!"));

	// 2. Print to Log
	UE_LOG(LogTemp, Warning, TEXT("Hello, world!"));

	// 3. Printing Values to specifiers
	int TestNumber = 42;
	bool TestBool = true; // 0 for False, 1 for True (Or any number other than zero)
	UE_LOG(LogTemp, Warning, TEXT("TestNumber: %i"), TestNumber);
	UE_LOG(LogTemp, Warning, TEXT("TestBool: %i"), TestBool);

	UE_LOG(LogTemp, Warning, TEXT("TestNumber: %i, TestBool: %i"), TestNumber, TestBool);
	UE_LOG(LogTemp, Warning, TEXT("TestBool: %i, TestNumber: %i"), TestBool, TestNumber);

	// 4. Pointers and Crashes
	//int* TestNumberPtr = &TestNumber;
	//UE_LOG(LogTemp, Warning, TEXT("TestNumberPtr: %p"), TestNumberPtr);
	//UE_LOG(LogTemp, Warning, TEXT("TestNumberPtr: %p"), TestNumberPtr); //Trying to display the number

	//UE_LOG(LogTemp, Warning, TEXT("DereferencedNumberPtr: %i"), *TestNumberPtr); //Dereferencing the pointer to get the value it points to

	// 5. Causing an intentional crash to deal nullptr
	/*TestNumberPtr = nullptr;*/
	//int TestCrash = *TestNumberPtr; 
	//UE_LOG(LogTemp, Warning, TEXT("TestNumberPtr: %i"), TestCrash);

	// 6. Printing a String
	//FString TestString = TEXT("Hello, Unreal!");
	//UE_LOG(LogTemp, Warning, TEXT("TestString: %s"), *TestString);

	//// 7. Modern Unreal way of logging without specifiers
	//UE_LOGFMT(LogTemp, Warning, "TestNumber: {0}, TestBool: {1}, TestString: {2}", TestNumber, TestBool, *TestString, "This is my String ya gabbagool");

}

void AGravityGunGameMode::AddScore(int32 Points)
{
	CurrentScore += Points;
}