#ifndef APP_H
#define APP_H

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <map>
#include <thread>

using std::map;
using std::queue;
using std::vector;
using std::condition_variable;
using std::mutex;
using std::pair;
using std::unique_lock;

typedef vector<unsigned char> DataType;
typedef pair<size_t, DataType> ResultType;

class App
{
    bool m_producerFinished;
    map<std::thread::id, ResultType> m_threadId_Results;

    queue<DataType> m_queue;
    condition_variable m_cv;
    mutex m_mutex;

public:
    App();
    ~App();

    void produce(std::function<DataType()> fun, size_t recordsNumber);
    ResultType consume();
    void stop();
};

#endif // APP_H
