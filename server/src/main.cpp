/*
 * KNP module 2 Sample starter code using Restinio library (HTTP with C++) for Client-Server
 * and you can also use this in your PRJ3 project
 * Underviser: "Jenny" Jung Min Kim
 *
 * Purpose:
 *   Minimal example showing how to use the Restinio library to handle HTTP protocol and
 *   HTTP-based Client–Server communication.
 *   (You’ll learn HTTP and implementations in detail in November in the KNP course.)
 *
 * IMPORTANT — Adapt for your PRJ3 project:
 *   ( You will learn these steps in KNP module 2 gradually)
 *   1) Replace book_t/book_collection/book_collection_t/book_handler_t
 *      with your own data structure, collections, and handlers.
 *   2) Update the initial values in main() to match your data and logic.
 *   3) Add the HTTP endpoints you need (GET/POST/PUT/DELETE, etc.).
 */

#include <iostream>
#include <restinio/all.hpp>
#include <json_dto/pub.hpp>
#include <cstdio>
#include <fstream>
#include "file_handler.h"

namespace rr = restinio::router;
using router_t = rr::express_router_t<>;

int playlist_count = 0;

auto server_handler(FileHandler &file_handler)
{

	auto router = std::make_unique<router_t>();

	auto by = [&](auto method)
	{
		using namespace std::placeholders;
		return std::bind(method, file_handler, _1, _2);
	};

	router->http_get("/", by(&FileHandler::music_list));

	// Call to get current song
	router->http_get("/get_current_song", [&](auto req, auto params)
					 {
		try
		{
			std::ifstream file(file_handler.songs[playlist_count].getPath(), std::ios::binary);
			if (!file.is_open())
			{
				throw std::runtime_error("Cant open file");
			}
			file.seekg(0, std::ios::end);
			std::size_t file_size = file.tellg();
			file.seekg(0, std::ios::beg);

			auto resp = req->create_response();
			resp.append_header("Content-Type", "audio/wav");
			resp.append_header("Content-Length", std::to_string(file_size));
			resp.append_header("Accept-Ranges", "bytes");

			resp.set_body(restinio::sendfile(file_handler.songs[playlist_count].getPath().string()));
			return resp.done();
		}
		catch (const std::exception &e)
		{
			return req->create_response(restinio::status_internal_server_error()).set_body(std::string("Error: ") + e.what()).done();
		} });

	// Call to get next song.
	router->http_get("/get_next_song/:id", [&](auto req, auto params)
					 {
		try
		{
			int id = restinio::cast_to<std::uint64_t>(params["id"]);
			std::cout << "next called with id: " << id << std::endl;
			// Gets the next song on the playlist.
			if(id == 0)
			{
				int tmp = (playlist_count + 1) % file_handler.songs.size();
				
				std::ifstream file(file_handler.songs[tmp].getPath(), std::ios::binary);
				if (!file.is_open())
				{
					throw std::runtime_error("Cant open file");
				}
				file.seekg(0, std::ios::end);
				std::size_t file_size = file.tellg();
				file.seekg(0, std::ios::beg);

				auto resp = req->create_response();
				resp.append_header("Content-Type", "audio/wav");
				resp.append_header("Content-Length", std::to_string(file_size));
				resp.append_header("Accept-Ranges", "bytes");

				resp.set_body(restinio::sendfile(file_handler.songs[tmp].getPath().string()));
				std::cout << "At song: "<< playlist_count << " in playlist\n";
				return resp.done();
			}
			//Gets song 2 places in fornt of current and increments current song.
			else if(id == 1)
			{
				int tmp = (playlist_count + 2) % file_handler.songs.size();
				
				std::ifstream file(file_handler.songs[tmp].getPath(), std::ios::binary);
				if(!file.is_open())
				{
					throw std::runtime_error("Cant open file");
				}
				file.seekg(0, std::ios::end);
				std::size_t file_size = file.tellg();
				file.seekg(0, std::ios::beg);

				auto resp = req->create_response();
				resp.append_header("Content-Type","audio/wav");
				resp.append_header("Content-Length",std::to_string(file_size));
				resp.append_header("Accept-Ranges", "bytes");

				resp.set_body(restinio::sendfile(file_handler.songs[tmp].getPath().string()));
				playlist_count = (playlist_count + 1) % file_handler.songs.size();
				std::cout << "At song: "<< playlist_count << " in playlist\n";
				return resp.done();
			}
			// Incalid parameter handler.
			else
			{
				return req->create_response(restinio::status_bad_request()).set_body("Error: Can only be called with params 0 or 1!").done();
			}
		}
		catch (const std::exception &e)
		{
			return req->create_response(restinio::status_internal_server_error()).set_body(std::string("Error: ") + e.what()).done();
		} });

	// Call to get previous song
	router->http_get("/get_previous_song/:id", [&](auto req, auto params)
					 {
		try
		{
			int id = restinio::cast_to<std::uint64_t>(params["id"]);
			std::cout << "previous called with id: " << id << std::endl;
			//Gets the previous song on the playlist
			if (id == 0)
			{
				int tmp = (playlist_count - 1) % file_handler.songs.size();
				if (tmp < 0)
					tmp += file_handler.songs.size();

				std::ifstream file(file_handler.songs[tmp].getPath(), std::ios::binary);
				if(!file.is_open())
				{
					throw std::runtime_error("Cant open file");
				}
				file.seekg(0, std::ios::end);
				std::size_t file_size = file.tellg();
				file.seekg(0, std::ios::beg);

				auto resp = req->create_response();
				resp.append_header("Content-Type","audio/wav");
				resp.append_header("Content-Length",std::to_string(file_size));
				resp.append_header("Accept-Ranges", "bytes");

				resp.set_body(restinio::sendfile(file_handler.songs[tmp].getPath().string()));
				std::cout << "At song: "<< playlist_count << " in playlist\n";
				return resp.done();
			}
			//Gets song 2 places behind current song and decremenst current song. 
			else if (id == 1)
			{	
				int tmp = (playlist_count - 2) % file_handler.songs.size();
				if (tmp < 0)
					tmp += file_handler.songs.size();

				std::ifstream file(file_handler.songs[tmp].getPath(), std::ios::binary);
				if(!file.is_open())
				{
					throw std::runtime_error("Cant open file");
				}
				file.seekg(0, std::ios::end);
				std::size_t file_size = file.tellg();
				file.seekg(0, std::ios::beg);

				auto resp = req->create_response();
				resp.append_header("Content-Type","audio/wav");
				resp.append_header("Content-Length",std::to_string(file_size));
				resp.append_header("Accept-Ranges", "bytes");

				resp.set_body(restinio::sendfile(file_handler.songs[tmp].getPath().string()));
				playlist_count = (playlist_count - 1) % file_handler.songs.size();
				if(playlist_count < 0)
					playlist_count += file_handler.songs.size();
				std::cout << "At song: "<< playlist_count << " in playlist\n";
				return resp.done();
			}
			else
			{
				return req->create_response(restinio::status_bad_request()).set_body("Error: Can only be called with params 0 or 1!").done();
			}
		}
		catch (const std::exception &e)
		{
			return req->create_response(restinio::status_internal_server_error()).set_body(std::string("Error: ") + e.what()).done();
		} });

	router->http_get("/chat", by(&FileHandler::on_live_update));


	router->http_post("/shuffle_playlist", [&](auto req, auto params)
					  {
		file_handler.shuffle();
		for (auto song: file_handler.songs)
		{
			std::cout << song.getTitle() << std::endl;
		}
		auto resp = req->create_response();
		return resp.done(); 
	});

	router->http_post("/error/:error", by(&FileHandler::on_error));

	return router;
}

//=========================*/
// Main entry point
//=========================*/
int main()
{
	using namespace std::chrono;

	try
	{
		using traits_t = restinio::traits_t<
			restinio::asio_timer_manager_t,
			restinio::single_threaded_ostream_logger_t,
			router_t>;

		FileHandler file_handler;

		//=========================*/
		// Run server
		// IMPORTANT:
		//   - "0.0.0.0" means server listens on ALL network interfaces.
		//   - Use Pi's IP + port 8080 to connect from another device.
		//=========================*/
		restinio::run(
			restinio::on_this_thread<traits_t>()
				.address("0.0.0.0") // For Pi: allow access from outside
				.port(8080)			// Default port, change if needed
				.request_handler(server_handler(file_handler))
				.read_next_http_message_timelimit(30s)
				.write_http_response_timelimit(60s)
				.handle_request_timeout(60s));
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}