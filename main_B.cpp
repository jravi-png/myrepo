
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <set>
#include <curl/curl.h>
#include <pugixml.hpp>
const int MAX_THREADS = 10; // You can adjust this number as per your system's capabilities
const std::string BASE_URL = "https://example.com"; // Start crawling from this URL

std::mutex mtx;
std::condition_variable cv;
std::queue<std::string> urlQueue;
std::set<std::string> visitedUrls;
bool stop = false;

// Function to fetch the content of a URL using libcurl
size_t writeCallback(char* contents, size_t size, size_t nmemb, std::string* userp)
{
    size_t realsize = size * nmemb;
    userp->append(contents, realsize);
    return realsize;
}

std::string fetchUrl(const std::string& url)
{
    CURL* curl;
    CURLcode res;
    std::string content;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            content.clear();
        }

        curl_easy_cleanup(curl);
    }

    return content;
}

// Function to parse HTML and extract links
std::vector<std::string> extractLinks(const std::string& content)
{
    std::vector<std::string> links;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(content.c_str());

    if (result) {
        for (const pugi::xml_node& node : doc.child("html").child("body").find_all_nodes("a")) {
            std::string href = node.attribute("href").value();
            if (href.size() > 0) {
                links.push_back(href);
            }
        }
    }

    return links;
}

void workerThread()
{
    while (!stop) {
        std::string url;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, []{ return !urlQueue.empty() || stop; });

            if (!urlQueue.empty()) {