// DiningPhilosophers.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <semaphore>
#include <vector>
#include <mutex>
#include <random>

enum class State
{
    THINKING = 0,  // philosopher is THINKING
    HUNGRY = 1,    // philosopher is trying to get forks
    EATING = 2,    // philosopher is EATING
};

std::mutex criticalRegionMtx;
std::mutex outputMtx;
std::vector<State> philosophersState;
std::vector<std::unique_ptr<std::binary_semaphore>> bothForksAvailable;


int leftNeighborIndex(int philosopherIndex, int philosophersCount) {
    return (philosopherIndex - 1 + philosophersCount) % philosophersCount;
}

int rightNeighborIndex(int philosopherIndex, int philosophersCount) {
    return (philosopherIndex + 1) % philosophersCount;
}

int randomInteger(int min, int max)
{
    static std::mt19937 rnd(std::time(nullptr));
    return std::uniform_int_distribution<>(min, max)(rnd);
}

void test(int philosopherIndex)
{
    if (philosophersState.at(philosopherIndex) == State::HUNGRY &&
        philosophersState.at(leftNeighborIndex(philosopherIndex, philosophersState.size())) != State::EATING &&
        philosophersState.at(rightNeighborIndex(philosopherIndex, philosophersState.size())) != State::EATING)
    {
        philosophersState.at(philosopherIndex) = State::EATING;
        bothForksAvailable.at(philosopherIndex)->release();
    }
}

void think(int philosopherIndex)
{
    int duration = randomInteger(500, 1000);
    outputMtx.lock();
    std::cout << "Philosopher " << philosopherIndex << " is thinking " << duration << "ms\n";
    outputMtx.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void takeForks(int philosopherIndex)
{
    criticalRegionMtx.lock();
    philosophersState.at(philosopherIndex) = State::HUNGRY;
    outputMtx.lock();
    std::cout << "\t\t" << "Philosopher" << philosopherIndex << " is hungry\n";
    outputMtx.unlock();
    test(philosopherIndex);
    criticalRegionMtx.unlock();
    bothForksAvailable.at(philosopherIndex)->acquire();
}

void eat(int philosopherIndex)
{
    int duration = randomInteger(500, 1000);
    outputMtx.lock();
    std::cout << "\t\t\t\t" << "Philosopher " << philosopherIndex << " is eating " << duration << "ms\n";
    outputMtx.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

void putForks(int philosopherIndex)
{
    criticalRegionMtx.lock();
    philosophersState.at(philosopherIndex) = State::THINKING;
    test(leftNeighborIndex(philosopherIndex, philosophersState.size()));
    test(rightNeighborIndex(philosopherIndex, philosophersState.size()));
    criticalRegionMtx.unlock();
}

void philosopher(int philosopherIndex)
{
    while (true)
    {                         
        think(philosopherIndex);             
        takeForks(philosopherIndex);
        eat(philosopherIndex);               
        putForks(philosopherIndex);
    }
}

int main()
{
    int philosophersCount;
    std::cout << "Pick a natural number, representing the number of philosophers: ";
    std::cin >> philosophersCount;

    for (int i = 0; i < philosophersCount; i++) {
        philosophersState.emplace_back(State::THINKING);
    }
    for (int i = 0; i < philosophersCount; i++) {
        bothForksAvailable.push_back(std::make_unique<std::binary_semaphore>(0));
    }

    std::vector<std::thread> philosophers; 
    for (int i = 0; i < philosophersCount; i++) {
        philosophers.emplace_back(philosopher, i);
    }
    for (auto& t : philosophers)
        t.join();
}