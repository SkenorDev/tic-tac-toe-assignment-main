#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"
#include <fstream>
#include <string>
#include <chrono>
#include <stack>
namespace ClassGame {
    struct logMessage{
    std::string type;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
};
    std::vector<logMessage> logs;
void logInfo(std::string message) {
    logMessage input{"Info", message, std::chrono::system_clock::now()};
    logs.push_back(input);
}
        void logError(std::string message){
        logMessage input{"Error", message, std::chrono::system_clock::now()};
        logs.push_back(input);
        }
        void logWarning(std::string message){
        logMessage input{"Warning", message, std::chrono::system_clock::now()};
        logs.push_back(input);
        }
        void logClear(){
            logs.clear();
        }
        //https://chatgpt.com/share/68e6b2d3-b828-8009-b4d4-c7eb45a3201a
        void writeLogsToFile() {
             auto now = std::chrono::system_clock::now();
             auto t = std::chrono::system_clock::to_time_t(now);
                 std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "logs_%Y-%m-%d_%H-%M-%S.txt", &tm);
    std::string filename = buffer;

    // Open file
    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        logError("Failed to open log file for writing.");
        return;
    }

    // Write each log entry
    for (const auto& log : logs) {
        auto time = std::chrono::system_clock::to_time_t(log.timestamp);
        std::string timeStr = std::ctime(&time);
        timeStr.pop_back(); // remove newline
        outFile << "[" << timeStr << "] " << log.type << ": " << log.message << "\n";
    }

    outFile.close();

    logInfo("Logs written to " + filename);
}
        //
        // our global variables
        //
        TicTacToe *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new TicTacToe();
            game->setUpBoard();
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                if (!game) return;
                if (!game->getCurrentPlayer()) return;
                
                ImGui::Begin("Settings");
                ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                ImGui::Text("Current Board State: %s", game->stateString().c_str());
                if (ImGui::Button("Enable Ai")){
                    // game->_gameOptions.AIPlaying = true;
                game->setAIPlayer(1);
                //game->getCurrentPlayer()->setAIPlayer(true);

            }
                if (gameOver) {
                    ImGui::Text("Game Over!");
                    ImGui::Text("Winner: %d", gameWinner);
                    if (ImGui::Button("Reset Game")) {
                        game->stopGame();
                        game->setUpBoard();
                        gameOver = false;
                        gameWinner = -1;
                    }
                    
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                game->drawFrame();
                ImGui::End();
                 ImGui::Begin("ImGui Logger");
            if (ImGui::Button("Clear")){
                logClear();
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Info")){
                logInfo("Test Info");
            }
            ImGui::SameLine();
            if (ImGui::Button("Test Warning")){
                logWarning("Warning Test");
            }
            ImGui::SameLine();
            if (ImGui::Button("Error Info")){
                logError("Error Info");
            }
            if (ImGui::Button("Write Logs to File")) {
                 writeLogsToFile();
}

            for (const auto& log : logs) {
                auto time = std::chrono::system_clock::to_time_t(log.timestamp);
                std::string timeStr = std::ctime(&time);
                timeStr.pop_back(); 
                ImVec4 color;
                if (log.type == "Error")
                    color = ImVec4(1, 0.3f, 0.3f, 1);
                else if (log.type == "Warning")
                    color = ImVec4(1, 1, 0.3f, 1);
                else
                color = ImVec4(0.5f, 0.9f, 1, 1);
                ImGui::TextColored(color,"[%s] %s: %s", timeStr.c_str(), log.type.c_str(), log.message.c_str());
}
ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}
