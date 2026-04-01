#include <string>
#include <filesystem>
#include <vector>
#include <iostream>
#include <map>
#include <restinio/all.hpp>
#include <json_dto/pub.hpp>
#include <restinio/websocket/websocket.hpp>

namespace fs = std::filesystem;
namespace rr = restinio::router;
namespace rws = restinio::websocket::basic;
using router_t = rr::express_router_t<>;
using traits_t = restinio::traits_t<restinio::asio_timer_manager_t, restinio::single_threaded_ostream_logger_t, router_t>;
using ws_registry_t = std::map<std::uint64_t, rws::ws_handle_t>;

struct Song
{
    private:
    std::string title;
    fs::path path;
public:
    Song(std::string title, fs::path path) : title(title), path(path) {};

    template <typename JSON_IO>
    void json_io(JSON_IO &io)
    {
        io &json_dto::mandatory("title", title);
    }
    std::string getTitle() const {return title;};
    fs::path getPath()const {return path;};
};

class FileHandler
{
    bool shuffled = false;
    ws_registry_t m_registry;
    std::vector<Song> get_songs();
    const std::vector<Song> original_playlist;
    template <typename RESP>
    static RESP init_resp(RESP resp)
    {
        resp
            .append_header("Server", "RESTinio sample server /v.0.6")
            .append_header_date_field()
            .append_header("Content-Type", "text/plain; charset=utf-8");
        return resp;
    }

    static void mark_as_bad_request(auto &resp) // (auto & resp) with C++20 , if C++17, it should be (RESP & resp)
    {
        resp.header().status_line(restinio::status_bad_request());
    }

    void sendMessage(const std::string& msg) const;

public:
    explicit FileHandler();
    std::vector<Song> songs;
    restinio::request_handling_status_t music_list(const restinio::request_handle_t &req, rr::route_params_t) const;
    void shuffle();
    restinio::request_handling_status_t on_live_update(const restinio::request_handle_t& req, rr::route_params_t params);
    restinio::request_handling_status_t on_error(const restinio::request_handle_t& req, rr::route_params_t params);
};