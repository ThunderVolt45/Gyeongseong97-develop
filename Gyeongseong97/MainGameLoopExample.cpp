// This is a hypothetical main game loop example for demonstration purposes.
// You would integrate this logic into your actual game loop.

#include "Narration.h"
#include <iostream>
#include <chrono>
#include <thread> // For std::this_thread::sleep_for for simulating time progression

int main()
{
	std::cout << "Starting hypothetical game loop simulation..." << std::endl;

	// 1. Create an instance of Narration (Boss Monster)
	// Example values: x=0, y=0, health=100, speed=1.0f, killScore=1000
	Narration boss(0, 0, 100, 1.0f, 1000);

	// Simulate game loop for a certain duration (e.g., 30 seconds)
	auto simulationStartTime = std::chrono::high_resolution_clock::now();
	auto currentTime = simulationStartTime;
	double simulationDuration = 30.0; // seconds

	int damageInterval = 5; // Damage boss every 5 seconds
	auto lastDamageTime = simulationStartTime;

	while (std::chrono::duration<double>(currentTime - simulationStartTime).count() < simulationDuration)
	{
		// Simulate a game tick/frame update
		std::cout << "\n--- Game Tick ---" << std::endl;

		// 2. Call Narration::Update() in each frame
	
boss.Update();

		// 3. Periodically call Narration::TakeDamage()
		if (std::chrono::duration<double>(currentTime - lastDamageTime).count() > damageInterval)
		{
		
boss.TakeDamage(30); // Deal 30 damage
			lastDamageTime = currentTime;
		}

		// Simulate some time passing
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		currentTime = std::chrono::high_resolution_clock::now();
	}

	std::cout << "\nHypothetical game loop simulation finished." << std::endl;

	return 0;
}
