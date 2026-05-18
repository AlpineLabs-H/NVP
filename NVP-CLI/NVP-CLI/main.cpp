#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "commands.h"

using json = nlohmann::json;

struct Config
{
    std::string apiUrl;
    std::string apiKey;
};

static const char* defaultApiUrl = "http://localhost:5000";

static std::string getConfigPath()
{
    if (const char* envPath = std::getenv("NVP_CONFIG"))
        return std::string(envPath);

    const char* home = std::getenv("HOME");
    if (!home)
        home = std::getenv("USERPROFILE");

    if (home)
        return std::string(home) + "/.nvpconfig";

    return ".nvpconfig";
}

static bool saveConfig(const Config& cfg)
{
    std::ofstream out(getConfigPath());
    if (!out)
        return false;

    json data = {
        {"apiUrl", cfg.apiUrl},
        {"apiKey", cfg.apiKey}
    };

    out << data.dump(2);
    return out.good();
}

static bool loadConfig(Config& cfg)
{
    std::ifstream in(getConfigPath());
    if (!in)
        return false;

    json data;
    in >> data;

    if (!data.contains("apiUrl") || !data.contains("apiKey"))
        return false;

    cfg.apiUrl = data.value("apiUrl", "");
    cfg.apiKey = data.value("apiKey", "");
    return !cfg.apiUrl.empty() && !cfg.apiKey.empty();
}

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    auto* response = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

static std::string urlEncode(CURL* curl, const std::string& value)
{
    char* escaped = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
    if (!escaped)
        return {};

    std::string result(escaped);
    curl_free(escaped);
    return result;
}

static bool httpGet(const std::string& url, const std::string& apiKey, std::string& response, long& httpCode)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, ("X-Api-Key: " + apiKey).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

static bool httpPostJson(const std::string& url, const std::string& apiKey, const std::string& body, std::string& response, long& httpCode)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("X-Api-Key: " + apiKey).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

static bool ensureConfig(Config& cfg)
{
    if (loadConfig(cfg))
        return true;

    std::cerr << "Configuration is missing or invalid. Run 'nvp init' first.\n";
    return false;
}

static bool queryPackages(const Config& cfg, const std::string& name, const std::string& os, const std::string& nvd, json& packages)
{
    CURL* curl = curl_easy_init();
    if (!curl)
        return false;

    std::string requestUrl = cfg.apiUrl + "/package/find/" + urlEncode(curl, name) + "/" + urlEncode(curl, os) + "/" + urlEncode(curl, nvd);
    curl_easy_cleanup(curl);

    std::string response;
    long statusCode = 0;
    if (!httpGet(requestUrl, cfg.apiKey, response, statusCode))
    {
        std::cerr << "HTTP request failed when querying packages.\n";
        return false;
    }

    if (statusCode != 200)
    {
        std::cerr << "API returned status " << statusCode << ": " << response << "\n";
        return false;
    }

    try
    {
        packages = json::parse(response);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Failed to parse response: " << ex.what() << "\n";
        return false;
    }

    return true;
}

static void printPackage(const json& item)
{
    std::cout << "Package: " << item.value("name", "<unknown>") << "\n";
    std::cout << "  id: " << item.value("id", "<unknown>") << "\n";
    std::cout << "  description: " << item.value("description", "<none>") << "\n";
    std::cout << "  url: " << item.value("URL", "<none>") << "\n";
    std::cout << "  author: " << item.value("author", "<unknown>") << "\n";
    std::cout << "  OS: " << item.value("OS", "<unknown>") << "\n";
    std::cout << "  NVD: " << item.value("NVD", "<unknown>") << "\n";
}

void printHelp()
{
    std::cout << "nvp - Neovim Package Manager CLI\n\n";
    std::cout << "Usage:\n";
    std::cout << "  nvp init [apiUrl] [apiKey]            Initialize CLI configuration\n";
    std::cout << "  nvp search <name> <OS> <NVD>         Search packages\n";
    std::cout << "  nvp install <name> <OS> <NVD>        Show install details for a package\n";
    std::cout << "  nvp publish <id> <name> <description> <URL> <author> <OS> <NVD>\n";
    std::cout << "                                       Publish a new package to the API\n";
    std::cout << "  nvp help                             Show this message\n";
}

void cmd_init(int argc, char* argv[])
{
    Config cfg;
    cfg.apiUrl = defaultApiUrl;

    if (argc >= 3 && argv[2] != nullptr)
        cfg.apiUrl = argv[2];

    std::cout << "NVP CLI initialization\n";
    std::cout << "API URL [" << cfg.apiUrl << "]: ";

    std::string input;
    std::getline(std::cin, input);
    if (!input.empty())
        cfg.apiUrl = input;

    if (argc >= 4 && argv[3] != nullptr)
    {
        cfg.apiKey = argv[3];
    }
    else
    {
        std::cout << "API key: ";
        std::getline(std::cin, cfg.apiKey);
    }

    if (cfg.apiKey.empty())
    {
        std::cerr << "API key is required. Visit " << cfg.apiUrl << "/auth/login to obtain a key.\n";
        return;
    }

    if (!saveConfig(cfg))
    {
        std::cerr << "Failed to save configuration to " << getConfigPath() << "\n";
        return;
    }

    std::cout << "Configuration saved to " << getConfigPath() << "\n";
}

void cmd_search(const char* query, const char* os, const char* nvd)
{
    Config cfg;
    if (!ensureConfig(cfg))
        return;

    json packages;
    if (!queryPackages(cfg, query, os, nvd, packages))
        return;

    if (!packages.is_array() || packages.empty())
    {
        std::cout << "No packages found for '" << query << "' on " << os << " / " << nvd << ".\n";
        return;
    }

    std::cout << "Found " << packages.size() << " package(s):\n";
    for (const auto& item : packages)
    {
        printPackage(item);
        std::cout << "\n";
    }
}

void cmd_install(const char* query, const char* os, const char* nvd)
{
    Config cfg;
    if (!ensureConfig(cfg))
        return;

    json packages;
    if (!queryPackages(cfg, query, os, nvd, packages))
        return;

    if (!packages.is_array() || packages.empty())
    {
        std::cout << "Package not found: '" << query << "' on " << os << " / " << nvd << ".\n";
        return;
    }

    const auto& packageItem = packages.front();
    std::cout << "Install target:\n";
    printPackage(packageItem);
    std::cout << "\n";
    std::cout << "Install instructions:\n";
    std::cout << "  1. Add the package URL to your Neovim plugin manager.\n";
    std::cout << "  2. Follow the repository URL: " << packageItem.value("URL", "<url>") << "\n";
}

void cmd_publish(int argc, char* argv[])
{
    Config cfg;
    if (!ensureConfig(cfg))
        return;

    std::string id;
    std::string name;
    std::string description;
    std::string url;
    std::string author;
    std::string os;
    std::string nvd;

    if (argc == 9)
    {
        id = argv[2];
        name = argv[3];
        description = argv[4];
        url = argv[5];
        author = argv[6];
        os = argv[7];
        nvd = argv[8];
    }
    else if (argc == 2)
    {
        std::cout << "Publish a new package to the API\n";
        std::cout << "id: ";
        std::getline(std::cin, id);
        std::cout << "name: ";
        std::getline(std::cin, name);
        std::cout << "description: ";
        std::getline(std::cin, description);
        std::cout << "URL: ";
        std::getline(std::cin, url);
        std::cout << "author: ";
        std::getline(std::cin, author);
        std::cout << "OS: ";
        std::getline(std::cin, os);
        std::cout << "NVD: ";
        std::getline(std::cin, nvd);
    }
    else
    {
        std::cerr << "Usage: nvp publish <id> <name> <description> <URL> <author> <OS> <NVD>\n";
        return;
    }

    if (id.empty() || name.empty() || url.empty() || author.empty() || os.empty() || nvd.empty())
    {
        std::cerr << "All fields are required to publish a package.\n";
        return;
    }

    json body = {
        {"id", id},
        {"name", name},
        {"description", description},
        {"URL", url},
        {"author", author},
        {"OS", os},
        {"NVD", nvd}
    };

    std::string response;
    long statusCode = 0;
    if (!httpPostJson(cfg.apiUrl + "/package/add", cfg.apiKey, body.dump(), response, statusCode))
    {
        std::cerr << "Failed to publish package.\n";
        return;
    }

    if (statusCode == 200)
    {
        std::cout << "Package published successfully.\n";
    }
    else
    {
        std::cerr << "API returned " << statusCode << ": " << response << "\n";
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printHelp();
        return 0;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    int result = 0;

    if (strcmp(argv[1], "help") == 0)
    {
        printHelp();
    }
    else if (strcmp(argv[1], "init") == 0)
    {
        cmd_init(argc, argv);
    }
    else if (strcmp(argv[1], "search") == 0)
    {
        if (argc != 5)
        {
            std::cerr << "Usage: nvp search <name> <OS> <NVD>\n";
            result = 1;
        }
        else
        {
            cmd_search(argv[2], argv[3], argv[4]);
        }
    }
    else if (strcmp(argv[1], "install") == 0)
    {
        if (argc != 5)
        {
            std::cerr << "Usage: nvp install <name> <OS> <NVD>\n";
            result = 1;
        }
        else
        {
            cmd_install(argv[2], argv[3], argv[4]);
        }
    }
    else if (strcmp(argv[1], "publish") == 0)
    {
        cmd_publish(argc, argv);
    }
    else
    {
        std::cerr << "Unknown command: " << argv[1] << "\n";
        printHelp();
        result = 1;
    }

    curl_global_cleanup();
    return result;
}
