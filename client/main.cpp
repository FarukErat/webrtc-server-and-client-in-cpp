#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/bind/bind.hpp>
#include <chrono>
#include <thread>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = boost::asio::ip::tcp;

class WebSocketClient {
public:
    WebSocketClient(boost::asio::io_context& ioc, const std::string& host, unsigned short port)
        : resolver_(ioc), ws_(ioc), host_(host), port_(std::to_string(port)) {}

    void connect() {
        resolver_.async_resolve(host_, port_,
            boost::bind(&WebSocketClient::onResolve, this, boost::asio::placeholders::error, boost::asio::placeholders::results));
    }

private:
    tcp::resolver resolver_;
    websocket::stream<tcp::socket> ws_;
    std::string host_;
    std::string port_;

    void onResolve(const boost::system::error_code& ec, tcp::resolver::results_type results) {
        if (ec) {
            std::cerr << "Resolve failed: " << ec.message() << std::endl;
            return;
        }

        boost::asio::async_connect(ws_.next_layer(), results.begin(), results.end(),
            boost::bind(&WebSocketClient::onConnect, this, boost::asio::placeholders::error));
    }

    void onConnect(const boost::system::error_code& ec) {
        if (ec) {
            std::cerr << "Connect failed: " << ec.message() << std::endl;
            return;
        }

        ws_.async_handshake(host_, "/",
            boost::bind(&WebSocketClient::onHandshake, this, boost::asio::placeholders::error));
    }

    void onHandshake(const boost::system::error_code& ec) {
        if (ec) {
            std::cerr << "Handshake failed: " << ec.message() << std::endl;
            return;
        }

        std::cout << "Connected to the server" << std::endl;
        startSendLoop();
    }

    void startSendLoop() {
        while (true) {
            std::string message = "Hello, server!";
            ws_.write(boost::asio::buffer(message));

            std::cout << "Sent message to server: " << message << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

int main() {
    const std::string host = "signaling_server";
    const unsigned short port = 8321;

    try {
        boost::asio::io_context io_context;
        WebSocketClient client(io_context, host, port);
        client.connect();
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
