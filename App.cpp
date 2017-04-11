#include "App.h"
#include <chrono>
using namespace std::chrono_literals;

App::App() :
    m_producerFinished(false)
{
}

App::~App() {
    for (auto& data: m_threadId_Results) {
        std::cout << "Thread: "
                  << std::this_thread::get_id()
                  << ", processed: "
                  << data.second.first
                  << " records."
                  << std::endl;
    }
}

void App::produce(std::function<DataType ()> fun, size_t recordsNumber) {
    for (size_t i = 0; i < recordsNumber; ++i) {
        unique_lock<mutex> lock(m_mutex);

        m_queue.emplace(fun());
        lock.unlock();

        m_cv.notify_one();
    }
}

ResultType App::consume() {
    while (true) {
        unique_lock<mutex> lock(m_mutex);

        m_cv.wait(lock, [this]() {
            // true condition for exit from wait
            return !m_queue.empty() || m_producerFinished;
        });

        if (!m_queue.empty())
        {
            const DataType element(m_queue.front());
            m_queue.pop();
            auto& results = m_threadId_Results[std::this_thread::get_id()];
            ++results.first;
            auto& datas = results.second;
            if (datas.empty()) {
                datas = DataType(begin(element) + 1, end(element));
            } else {
                const DataType receivedData(begin(element) + 1, end(element));
                for (size_t i = 0; i < std::min(receivedData.size(), datas.size()); ++i) {
                    datas[i] ^= receivedData[i];
                }
            }
        }
        if (m_producerFinished && m_queue.empty()) {
            break;
        }
    }
    return m_threadId_Results[std::this_thread::get_id()];
}

void App::stop()
{
    m_producerFinished = true;
}
