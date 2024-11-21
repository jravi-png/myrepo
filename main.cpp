#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <curl/curl.h>

// Global thread pool size
const int THREAD_POOL_SIZE = 5;

std::queue<std::string> urlQueue;
std::mutex mtx;
std::condition_variable cv;
bool shouldStop = false;

void fetchURL(const std::string& url) {
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char* buffer, size_t size, size_t nmemb, std::ostream& out) {
            out.write(buffer, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &std::cout);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    cv.notify_one(); // Signal to the main thread that one URL is fetched
}

void workerThread() {
    while (!shouldStop) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{ return !urlQueue.empty() || shouldStop; });

            if (!urlQueue.empty()) {
                std::string url = urlQueue.front();
                urlQueue.pop();
                fetchURL(url);
            }
        }
    }
}

int main() {
    std::vector<std::thread> threads(THREAD_POOL_SIZE);

    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        threads[i] = std::thread(workerThread);
    }

    urlQueue.push("http://example.com");
    urlQueue.push("http://example.com/page1");
    urlQueue.push("http://example.com/page2");
    // Add more URLs to the queue

    // Simulate waiting for the thread pool to finish
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}