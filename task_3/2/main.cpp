#include <iostream>
#include <queue>
#include <future>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <fstream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>


template<typename T>
class Server
{
private:
    std::thread worker;
    std::atomic<bool> stop_server{false};

    std::mutex mtx;
    std::condition_variable cond;

    std::atomic<size_t> index_task{0};

    std::queue<std::pair<size_t, std::packaged_task<T()>>> tasks;
    std::unordered_map<size_t, std::future<T>> results;

public:
    Server() 
    {
        start();
    }

    void start()
    {
        stop_server.store(false);

        worker = std::thread([this]()
        {
            while (true)
            {
                std::pair<size_t, std::packaged_task<T()>> task_pair;

                {
                    std::unique_lock<std::mutex> lock(mtx);

                    cond.wait(lock, [this]
                    {
                        return stop_server.load() || !tasks.empty();
                    });

                    if (stop_server.load() && tasks.empty())
                        break;

                    task_pair = std::move(tasks.front());
                    tasks.pop();
                }

                task_pair.second(); 
            }
        });
    }

    void stop()
    {
        stop_server.store(true);
        cond.notify_all();

        if (worker.joinable())
            worker.join();
    }

    template<typename Func, typename... Args>
    size_t add_task(Func&& func, Args&&... args)
    {
        auto task = std::packaged_task<T()>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        );

        std::future<T> future = task.get_future();

        size_t id = index_task.fetch_add(1);

        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.push({id, std::move(task)});
            results[id] = std::move(future);
        }

        cond.notify_one();
        return id;
    }

    T request_result(size_t id)
    {
        std::future<T> future;

        {
            std::unique_lock<std::mutex> lock(mtx);

            auto it = results.find(id);
            if (it == results.end())
                throw std::runtime_error("Result not found");

            future = std::move(it->second);
            results.erase(it);
        }

        return future.get();
    }

    ~Server()
    {
        stop();
    }
};

thread_local std::mt19937 gen(std::random_device{}());
thread_local std::uniform_real_distribution<double> dist(1.0, 5.0);

template<typename T>
struct arity_helper;

template<typename R, typename... Args>
struct arity_helper<R(*)(Args...)>
{
    static constexpr size_t value = sizeof...(Args);
};

template<typename Func, typename T, std::size_t... Is>
auto invoke_with_vector(Func func, const std::vector<T>& args, std::index_sequence<Is...>)
{
    return func(args[Is]...);
}

template<typename T, typename Func>
void client(Server<T>& server, Func func, const std::string& filename, size_t N)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file\n";
        return;
    }

    constexpr size_t num_args = arity_helper<std::decay_t<Func>>::value;
    constexpr auto indices = std::make_index_sequence<num_args>{};

    std::vector<size_t> ids;

    for (size_t i = 0; i < N; i++)
    {
        std::vector<T> args;
        for (size_t j = 0; j < num_args; j++)
        {
            args.push_back(static_cast<T>(dist(gen)));
        }

        auto task = [func, args, indices]() -> T
        {
            return invoke_with_vector(func, args, indices);
        };

        ids.push_back(server.add_task(task));
    }

    for (size_t id : ids)
    {
        T res = server.request_result(id);
        file << res << "\n";
    }
}

template<typename T>
T my_sin(T x) 
{ 
    return std::sin(x); 
}

template<typename T>
T my_sqrt(T x) 
{ 
    return std::sqrt(x); 
}

template<typename T>
T my_pow(T x, T y) 
{ 
    return std::pow(x, y); 
}

void test_file(const std::string& filename)
{
    std::ifstream file(filename);
    double value;

    while (file >> value)
    {
        if (std::isnan(value) || std::isinf(value))
        {
            std::cout << "Error in " << filename << "\n";
            return;
        }
    }

    std::cout << filename << " OK\n";
}

int main(int argc, char* argv[])
{
    size_t N = 1000;
    if (argc == 2)
    {
        N = std::stoul(argv[1]);
    }
    Server<double> server;

    auto start = std::chrono::high_resolution_clock::now();

    std::thread t1([&]() { client(server, my_sin<double>,  "res_sin.txt", N); });
    std::thread t2([&]() { client(server, my_sqrt<double>, "res_sqrt.txt", N); });
    std::thread t3([&]() { client(server, my_pow<double>,  "res_pow.txt", N); });

    t1.join();
    t2.join();
    t3.join();

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << (end - start).count() << "seconds" << std::endl;

    server.stop();

    test_file("res_sin.txt");
    test_file("res_sqrt.txt");
    test_file("res_pow.txt");

    return 0;
}