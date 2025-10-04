#include <iostream>
#include <opencv2/opencv.hpp>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <atomic>
#include <csignal> // For signal handling
#include <chrono>  // For std::this_thread::sleep_for
#include <thread>  // For std::this_thread
#include <mutex>   // For std::mutex
#include <vector>

using namespace cv;
using namespace std;
using namespace Poco::Net;

std::atomic<bool> keepRunning(true);
std::mutex frameMutex;
Mat currentFrame;
std::vector<HTTPServerResponse*> clients;
std::mutex clientsMutex;

void signalHandler(int signal) {
    keepRunning = false;
}

class StreamRequestHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
        response.setChunkedTransferEncoding(true);
        response.setContentType("multipart/x-mixed-replace; boundary=frame");

        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(&response);

        while (::keepRunning.load()) {
            Mat frameToSend;
            {
                std::lock_guard<std::mutex> lock(frameMutex);
                frameToSend = currentFrame.clone(); // Make a copy to avoid race conditions
            }

            if (frameToSend.empty()) {
                cerr << "Error: Empty frame" << endl;
                break;
            }

            // Rotate the frame counter-clockwise by 90 degrees
            transpose(frameToSend, frameToSend);
            flip(frameToSend, frameToSend, 0);

            vector<uchar> buf;
            imencode(".jpg", frameToSend, buf);

            try {
                ostream& ostr = response.send();
                ostr << "--frame\r\n";
                ostr << "Content-Type: image/jpeg\r\n\r\n";
                ostr.write(reinterpret_cast<const char*>(buf.data()), buf.size());
                ostr << "\r\n";
            } catch (const Poco::Exception& e) {
                cerr << "Error sending frame: " << e.displayText() << endl;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust frame rate
        }

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.erase(std::remove(clients.begin(), clients.end(), &response), clients.end());
        }
    }
};

class StreamRequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest&) override {
        return new StreamRequestHandler;
    }
};

void sendToAllClients(Mat frame) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto& client : clients) {
        if (client) {
            vector<uchar> buf;
            imencode(".jpg", frame, buf);

            try {
                ostream& ostr = client->send();
                ostr << "--frame\r\n";
                ostr << "Content-Type: image/jpeg\r\n\r\n";
                ostr.write(reinterpret_cast<const char*>(buf.data()), buf.size());
                ostr << "\r\n";
            } catch (const Poco::Exception& e) {
                cerr << "Error sending frame to client: " << e.displayText() << endl;
                // Handle error (e.g., remove client from list)
            }
        }
    }
}

int main() {
    VideoCapture camera(0); // Open the default camera (camera index 0)
    if (!camera.isOpened()) {
        cerr << "Error: Could not open camera" << endl;
        return 1;
    }

    try {
        ServerSocket svs(8000); // Listen on port 8000
        StreamRequestHandlerFactory factory;
        HTTPServer srv(&factory, svs, new HTTPServerParams);
        srv.start();
        cout << "Server started" << endl;

        // Register signal handler to gracefully stop the server
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // Capture and send frames continuously
        while (keepRunning.load()) {
            Mat frame;
            camera.read(frame);

            {
                std::lock_guard<std::mutex> lock(frameMutex);
                currentFrame = frame.clone(); // Make a copy to avoid race conditions
            }

            sendToAllClients(frame); // Send frame to all connected clients

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust frame rate
        }

        // Stop the server
        srv.stop();
        cout << "Server stopped" << endl;
    } catch (const Poco::Exception& e) {
        cerr << "Exception: " << e.displayText() << endl;
    } catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
    } catch (...) {
        cerr << "Unknown exception occurred" << endl;
    }

    return 0;
}
