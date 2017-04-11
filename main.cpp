#include <iostream>
#include <random>

#include "App.h"

using std::thread;

DataType createArray(int seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<unsigned char> dist(1, 255);
    const unsigned char length = dist(gen);

    DataType array;
    array.resize(length + 1);
    // set first byte
    array[0] = length;

    // fill anyhow array
    for (size_t i = 1; i < length + 1; ++i) {
        array[i] = i;
    }
    return array;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cout << "Usage: "
                  << "first arg: seed, "
                  << "second arg: thread count, "
                  << "third arg: records per thread\n";
        return -1;
    }
    const int seed(atoi(argv[1]));
    const size_t threadCount(atoi(argv[2]));
    const size_t recordsNumber(atoi(argv[3]));

    App app;
    vector<thread> producers;
    vector<thread> consumers;
    for (size_t i = 0; i < threadCount; ++i) {
        producers.emplace_back(thread(&App::produce, &app, std::bind(&createArray, seed), recordsNumber));
    }
    for (size_t i = 0; i < threadCount; ++i) {
        consumers.emplace_back(thread(&App::consume, &app));
    }
    app.stop();
    for (auto& producerThread: producers) {
        producerThread.join();
    }

    for (auto& consumerThread: consumers) {
        consumerThread.join();
    }

    return 0;
}
