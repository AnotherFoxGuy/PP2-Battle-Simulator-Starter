#pragma once

#include <thread>
#include <future>
#include <vector>
#include <deque>

namespace PP2 {

    class ThreadPool; //Forward declare

    class Worker;

    class Worker {
    public:
        //Instantiate the worker class by passing and storing the threadpool as a reference
        Worker(ThreadPool &s) : pool(s) {}

        inline void operator()();

    private:
        ThreadPool &pool;
    };

    class ThreadPool {
    public:
        ThreadPool(size_t numThreads) : stop(false) {
            for (size_t i = 0; i < numThreads; ++i)
                workers.push_back(std::thread(Worker(*this)));
        }

        ~ThreadPool() {
            stop = true; // stop all threads
            condition.notify_all();

            for (auto &thread : workers)
                thread.join();
        }

        template<class T>
        auto enqueue(T task) -> std::future<decltype(task())> {
            //Wrap the function in a packaged_task so we can return a future object
            auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));

            //Scope to restrict critical section
            {
                //lock our queue and add the given task to it
                std::unique_lock<std::mutex> lock(queue_mutex);

                tasks.push_back([=] {
                    (*wrapper)();
                });
            }

            //Wake up a thread to start this task
            condition.notify_one();

            return wrapper->get_future();
        }

    private:
        friend class Worker; //Gives access to the private variables of this class

        std::vector<std::thread> workers;
        std::deque<std::function<void()>> tasks;

        std::condition_variable condition; //Wakes up a thread when work is available

        std::mutex queue_mutex; //Lock for our queue
        bool stop = false;
    };

    inline void Worker::operator()() {
        std::function<void()> task;
        while (true) {
            //Scope to restrict critical section
            //This is important because we don't want to hold the lock while executing the task,
            //because that would make it so only one task can be run simultaneously (aka sequantial)
            {
                std::unique_lock<std::mutex> locker(pool.queue_mutex);

                //Wait until some work is ready or we are stopping the threadpool
                //Because of spurious wakeups we need to check if there is actually a task available or we are stopping
                pool.condition.wait(locker, [=] { return pool.stop || !pool.tasks.empty(); });

                if (pool.stop) break;

                task = pool.tasks.front();
                pool.tasks.pop_front();
            }

            task();
        }
    }

} // namespace PP2