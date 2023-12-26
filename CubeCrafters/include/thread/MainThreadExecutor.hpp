#ifndef MAIN_THREAD_EXECUTOR
#define MAIN_THREAD_EXECUTOR

#include <functional>
#include <queue>
#include <mutex>

namespace MainThreadExecutor 
{
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;

    static void QueueTask(const std::function<void()>& task) 
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }

    static void UpdateTasks() 
    {
        while (true) 
        {
            std::function<void()> task;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (taskQueue.empty())
                    break;

                task = taskQueue.front();
                taskQueue.pop();
            }

            if (task)
                task();
        }
    }
};

#endif // !MAIN_THREAD_EXECUTOR