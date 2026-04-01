#include <iostream>
#include <string>
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <ostream>

namespace fs = std::filesystem;

class ServerComs
{
    int current_song = 0;
    const std::string server_url;
    bool shuffled;
    CURL* curl;
    static size_t write_callback(void* ptr, size_t size, size_t nmemb, void* userdata);
    bool clientStartup(const fs::path* song1_ptr);
    bool getNextSong(fs::path song_override, bool increment = true);
    bool getCurrentSong(fs::path song_override);
    bool getPreviousSong(fs::path song_override, bool decrement = true);
    const fs::path songs[3] = {"songs/song_1.wav", "songs/song_2.wav", "songs/song_3.wav"};
public:
    ServerComs(std::string server_url);
    ~ServerComs();
    bool shufflePlaylist();
    bool getSuffled();
    bool writeError(std::string msg);
    fs::path getCurrent();
    fs::path getNext();
    fs::path getPrevious();
};
 
