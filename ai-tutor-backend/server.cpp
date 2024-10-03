#include "crow.h"
#include "crow/websocket.h"
#include <string>
#include <vector>
#include <thread>

std::vector<crow::websocket::connection*> clients;

void broadcast(const std::string& message) {
    for (auto client : clients) {
        client->send_text(message);
    }
}

std::string generate_ai_response(const std::string& input) {
    // TODO: Integrate with your AI model here
    return "AI: " + input;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/ws")
        .websocket()
        .onopen([&](crow::websocket::connection& conn) {
            clients.push_back(&conn);
            std::cout << "New client connected" << std::endl;
        })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
            clients.erase(
                std::remove(clients.begin(), clients.end(), &conn),
                clients.end());
            std::cout << "Client disconnected" << std::endl;
        })
        .onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary) {
            if (!is_binary) {
                std::string response = generate_ai_response(data);
                broadcast(response);
            }
        });

    CROW_ROUTE(app, "/")([]() {
        return crow::response(200, "Server is running");
    });

    app.port(8080).multithreaded().run();
}