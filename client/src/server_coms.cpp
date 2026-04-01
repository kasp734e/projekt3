#include "server_coms.h"

namespace fs = std::filesystem;

size_t ServerComs::write_callback(void *ptr, size_t size, size_t nmemb, void
*userdata)
{
    std::ofstream *out = static_cast<std::ofstream *>(userdata);
    size_t total_size = size * nmemb;
    out->write(static_cast<const char *>(ptr), total_size);
    return total_size;
}

ServerComs::ServerComs(std::string server_url) : server_url(server_url)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if(!clientStartup(songs))
    {
        perror("Client start up failed");
        //error handling WIP
    }
}

ServerComs::~ServerComs()
{
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

bool ServerComs::clientStartup(const fs::path *song1_ptr)
{
    if (!getCurrentSong(song1_ptr[0]))
    {
        perror("Client start up");
        return false;
    }
    std::cout << "[Upstart]: Finsihed sending current song\n";
    if (!getNextSong(song1_ptr[1], false))
    {
        perror("Client start up");
        return false;
    }
    std::cout << "[Upstart]: Finsihed sending next song\n";
    if (!getPreviousSong(song1_ptr[2], false))
    {
        perror("Client start up");
        return false;
    }
    current_song = 0;
    std::cout << "[Upstart]: Finsihed sending previous song\n";
    return true;
}

bool ServerComs::getNextSong(fs::path song_override, bool increment)
{

    curl = curl_easy_init();
    if (!curl)
    {
        perror("Curl init");
        return false;
    }

    std::string url = server_url + "/get_next_song/" +
std::to_string((int)increment);

    std::ofstream out_file(song_override, std::ios::binary);
    if (!out_file.is_open())
    {
        perror("Failed to open song file");
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    out_file.close();
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
    {
        perror("Curl exit: \n");
        perror(curl_easy_strerror(res));
        return false;
    }
    // std::cout << "finished sending song\n";
    return true;
}

bool ServerComs::getCurrentSong(fs::path song_override)
{

    curl = curl_easy_init();
    if (!curl)
    {
        perror("Curl init");
        return false;
    }

    std::string url = server_url + "/get_current_song";

    std::ofstream out_file(song_override, std::ios::binary);
    if (!out_file.is_open())
    {
        perror("Failed to open song file");
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    out_file.close();
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
    {
        perror(curl_easy_strerror(res));
        return false;
    }
    // std::cout << "finished sending song\n";
    return true;
}

bool ServerComs::getPreviousSong(fs::path song_override, bool decrement)
{

    curl = curl_easy_init();
    if (!curl)
    {
        perror("Curl init");
        return false;
    }

    std::string url = server_url + "/get_previous_song/" +
std::to_string((int)decrement);

    std::ofstream out_file(song_override, std::ios::binary);
    if (!out_file.is_open())
    {
        perror("Failed to open song file");
        curl_easy_cleanup(curl);
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    out_file.close();
    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
    {
        perror("Curl exit: \n");
        perror(curl_easy_strerror(res));
        return false;
    }
    // std::cout << "finished sending song\n";
    return true;
}

bool ServerComs::shufflePlaylist()
{

    curl = curl_easy_init();
    if (!curl)
    {
        perror("Curl init");
        return false;
    }

    std::string url = server_url + "/shuffle_playlist";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
    {
        perror("failed to shuffle: \n");
        perror(curl_easy_strerror(res));
        return false;
    }
    return true;
}

bool ServerComs::getSuffled() { return shuffled; }

bool ServerComs::writeError(std::string msg)
{
    curl = curl_easy_init();
    if (!curl)
    {
        perror("Curl init");
        return false;
    }

    std::string url = server_url + "/error/" + msg;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    if (res != CURLE_OK)
    {
        perror("failed to write error: \n");
        perror(curl_easy_strerror(res));
        return false;
    }
    return true;
}

fs::path ServerComs::getCurrent(){
    return songs[current_song];
}

fs::path ServerComs::getNext(){

    int tmp = (current_song + 2) % 3;

    if(!getNextSong(songs[tmp]))
    {
        perror("Client get next song failed");
        //error handling WIP
    }
    current_song = (current_song + 1) % 3;
    return songs[current_song];
}

fs::path ServerComs::getPrevious(){
    int tmp = (current_song + 1) % 3;

    if(!getPreviousSong(songs[tmp]))
    {
        perror("Client get next song failed");
        //error handling WIP
    }
    current_song = (current_song + 2) % 3;
    return songs[current_song];
}
