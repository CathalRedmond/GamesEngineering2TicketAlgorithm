#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <SFML/Graphics.hpp>

const int N = 10;
std::atomic_int currentNumber;
std::atomic_int nextNumber;
std::atomic_int turn[N] = { 0,0,0,0,0,0,0,0,0,0 };
int threadNumber;
std::mutex mutex;

sf::Vector2f playerPosition;

enum class SystemType
{
    Physics,
    InputManager,
    Render,
    Collision
};


void updatePlayerPosition(sf::Vector2f t_newPosition)
{
    playerPosition = t_newPosition;
}

sf::Vector2f getPlayerPosition()
{
    return playerPosition;
}

void outputPosition()
{
    std::string xPos = std::to_string(playerPosition.x);
    std::string yPos = std::to_string(playerPosition.y);
    std::cout << "Position: \nX: " << xPos << ", Y: " << yPos << std::endl;
}

void ticketAlgorithm(SystemType t_updateType)
{
    int index = threadNumber++;

    mutex.lock();
    std::cout << "Current Thread: " << index << std::endl;
    mutex.unlock();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true)
    {
        turn[index] = currentNumber.fetch_add(1);

        mutex.lock();
        std::cout << "Current Thread: " << index << "\nTicket No.: " << turn[index] << std::endl;
        mutex.unlock();
        
        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (turn[index] != nextNumber)
        {
            continue;
        }

        mutex.lock();
        std::cout << "Current Thread: " << index << std::endl;
        mutex.unlock();

        switch (t_updateType)
        {
        case SystemType::Physics:
            updatePlayerPosition(getPlayerPosition() + sf::Vector2f(0, 9.8f));
            break;
        case SystemType::InputManager:
            updatePlayerPosition(getPlayerPosition() + sf::Vector2f(1, 1));
            break;
        case SystemType::Render:
            updatePlayerPosition(getPlayerPosition() + sf::Vector2f(0.5, 0.1));
            break;
        case SystemType::Collision:
            updatePlayerPosition(getPlayerPosition() + sf::Vector2f(-5, -5));
            break;
        default:
            break;
        }
        
        nextNumber++;

        mutex.lock();
        outputPosition();
        mutex.unlock();
    }
}


int main()
{
    srand(time(NULL));

    playerPosition = sf::Vector2f(0, 0);


    //Physics System 
    SystemType type = SystemType::Physics;
    std::thread physicsThread = std::thread(ticketAlgorithm, std::ref(type));
    //Input Manager System
    type = SystemType::InputManager;
    std::thread inputManagerThread = std::thread(ticketAlgorithm, std::ref(type));
    //Collision System
    type = SystemType::Collision;
    std::thread collisionThread = std::thread(ticketAlgorithm, std::ref(type));
    //Render System
    type = SystemType::Render;
    std::thread renderThread = std::thread(ticketAlgorithm, std::ref(type));

    physicsThread.join();
    inputManagerThread.join();
    collisionThread.join();
    renderThread.join();
}