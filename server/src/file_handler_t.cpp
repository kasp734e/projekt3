#include "file_handler.h"

namespace fs = std::filesystem;

std::vector<Song> FileHandler::get_songs()
{
    std::string music_path = "./music";
    std::vector<Song> tmp;

    for (auto &song : fs::directory_iterator(music_path))
    {
        tmp.push_back(Song(song.path().filename().string(), song.path()));
    }
    return tmp;
}

FileHandler::FileHandler() : original_playlist(get_songs())
{
    songs = original_playlist;
    for (auto &song : songs)
    {
        std::cout << song.getTitle() << std::endl;
    }
}

restinio::request_handling_status_t FileHandler::music_list(const restinio::request_handle_t &req, rr::route_params_t) const
{
    auto resp = init_resp(req->create_response());
    resp.set_body("Music on playlist:\n");
    for (auto &song : songs)
    {
        resp.append_body(song.getTitle());
        resp.append_body("\n");
    }

    return resp.done();
}

void FileHandler::shuffle(){
    fs::path playing_song;
    if (shuffled)
    {
        songs = original_playlist;
        shuffled = false;
        return;
    }
    songs.clear();
    srand(time(NULL));
    int size = original_playlist.size();
    bool added[size] = {};

    while(size > songs.size())
    {
        int rnd;
        while(true)
        {
            rnd = rand() % size;
            if(added[rnd])
                continue;
            break;
        }

        songs.push_back(original_playlist[rnd]);
        added[rnd] = true;
    }
    shuffled = true;
}

void FileHandler::sendMessage(const std::string& msg) const
{
    for(auto [k, v] : m_registry)
		{
			v->send_message(rws::final_frame, rws::opcode_t::text_frame, msg);
		}
}

restinio::request_handling_status_t FileHandler::on_live_update(const restinio::request_handle_t& req, rr::route_params_t params)
{
    if(restinio::http_connection_header_t::upgrade == req->header().connection())
		{
			std::shared_ptr<rws::ws_t> wsh = rws::upgrade<traits_t>(*req, rws::activation_t::immediate, 
				[&](auto wsh, auto m){
				if(rws::opcode_t::text_frame == m->opcode() ||
					rws::opcode_t::binary_frame == m->opcode() ||
					rws::opcode_t::continuation_frame == m->opcode())
				{
					wsh->send_message(*m);
				}
				else if(rws::opcode_t::ping_frame == m->opcode())
				{
					auto resp = *m;
					resp.set_opcode(rws::opcode_t::pong_frame);
					wsh->send_message(resp);
				}
				else if(rws::opcode_t::connection_close_frame == m->opcode())
				{
					m_registry.erase(wsh->connection_id());
				}
			});
			m_registry.emplace(wsh->connection_id(), wsh);

			return restinio::request_accepted();
		}
		return restinio::request_rejected();
}

restinio::request_handling_status_t FileHandler::on_error(const restinio::request_handle_t& req, rr::route_params_t params)
{
    std::string error = restinio::cast_to<std::string>(params["error"]);
    sendMessage("[CLIENT ERROR]: " + error);
    auto resp = req->create_response();
	return resp.done();
}