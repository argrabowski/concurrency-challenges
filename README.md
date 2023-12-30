# Thread Synchronization and Concurrency

This repository contains two projects focusing on synchronization and concurrency in C programming. The projects provide hands-on experience in creating threads, using randomness in delays to test scheduling, and solving synchronization problems using mutexes, condition variables, and semaphores.

## Summer Spectacular

### Overview
The "Summer Spectacular" project simulates an event organized by CircusJazz, LLC. Performers, including Flamenco dancers, torch jugglers, and duets, must take the stage independently, creating an improvisational masterpiece.

### Features
- Synchronization using mutexes and condition variables or semaphores.
- Safety constraints for performers on stage.
- Maximum parallelism for a spectacular performance.
- Fair performance opportunities for all performer types.
- Performance time variability and bounds.
- Pre-performance napping for ready performers.

### Implementation
- Simulated performances using the sleep() call with random wait times.
- Created threads for each performer type.
- Synchronized performers using semaphores.

## Shipping Shape-Up

### Overview
The "Shipping Shape-Up" project addresses challenges faced by shipping company FedOops. With a customer-centric initiative, logistics workers in color-coded teams compete to process packages efficiently, involving customers in the delivery process.

### Features
- Customer-dictated processing steps.
- Worker incentives and competition.
- Efficient parallelism while avoiding deadlock or starvation.
- Customer-centric initiative allowing customers to omit certain steps.

### Implementation
- Created a simulator for the FedOops distribution center with a Pile of Pending Packages (PPP).
- Represented packages as a queue with random sets of instructions.
- Synchronized workers with a combination of mutexes and condition variables.

## Usage
1. Clone the repository.
2. Navigate to the respective project directory.
3. Compile and run the C program.
