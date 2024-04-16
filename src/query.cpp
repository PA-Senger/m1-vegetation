#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include "query.hpp"

// Constructor definition
Query::Query(double Alat, double Alon, double Blat, double Blon)
{
    min_lat = std::min(Alat, Blat);
    max_lat = std::max(Alat, Blat);
    min_lon = std::min(Alon, Blon);
    max_lon = std::max(Alon, Blon);
}

// Destructor definition
Query::~Query() {}

// Callback function to receive response data
size_t write_callback(char *ptr, size_t size, size_t nmemb, std::string *data)
{
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

// Perform the Overpass query
void Query::perform_query()
{
    CURL *curl;
    CURLcode res;
    std::string response_data;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
        // Set the Overpass API endpoint URL
        curl_easy_setopt(curl, CURLOPT_URL, "http://overpass-api.de/api/interpreter");

        // Ensure coordinates are within valid range
        if (min_lat < -180.0 || min_lat > 180.0 || max_lat < -180.0 || max_lat > 180.0 ||
            min_lon < -180.0 || min_lon > 180.0 || max_lon < -180.0 || max_lon > 180.0)
        {
            std::cerr << "Error: Bounding box coordinates out of range (-180.0 to 180.0)" << std::endl;
            return;
        }

        // Set the Overpass query with the bounding box
        std::string query = "[out:json]; (node(" + std::to_string(min_lat) + "," +
                            std::to_string(min_lon) + "," +
                            std::to_string(max_lat) + "," +
                            std::to_string(max_lon) +
                            ")[\"natural\"=\"tree\"];); out;";

        std::cout << "Query: " << query << std::endl;

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());

        // Set callback function to receive response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Save the response data to a file
            std::ofstream output_file("query_result.json");
            if (output_file.is_open())
            {
                output_file << response_data;
                output_file.close();
                std::cout << "Query result saved to query_result.json" << std::endl;
            }
            else
            {
                std::cerr << "Failed to open output file" << std::endl;
            }
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize libcurl" << std::endl;
    }

    // Cleanup libcurl
    curl_global_cleanup();
}

nlohmann::json Query::get_query_result()
{
    std::ifstream inputFile("query_result.json");
    nlohmann::json jsonData;
    inputFile >> jsonData;
    return jsonData;
}