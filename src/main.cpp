#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <ctime>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

// Global paramters to check in onMessage
double int_cte = 0; // Integral of CTE
double prev_cte = 0;// Previous CTE
std::chrono::_V2::system_clock::time_point t_start; // To check the time b/w calls



int main()
{
  uWS::Hub h;

  PID pid;

  // Set initial values as Kp=1, Kd=0.5 and Ki=0
  pid.Init(1,0,0.5);
  // Start the clock
  t_start = std::chrono::high_resolution_clock::now();


  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

          pid.UpdateError(cte);

          // Calculate the time passed
          auto t_end = std::chrono::high_resolution_clock::now();
          double passed = std::chrono::duration<double, std::milli>(t_end-t_start).count();
          t_start  = t_end;
          passed /= 1000.0;
          std:: cout << "Total seconds passed:" << passed << std::endl;


          // ------------------------
          //
          // PID Algorithm
          //
          // ------------------------
          double diff_cte = (cte - prev_cte)/passed;
          prev_cte = cte;
          int_cte += (cte*passed);
          steer_value = -pid.Kp * cte - pid.Kd * diff_cte - pid.Ki * int_cte;

          // Debug statements
          std::cout << "Current Angle: " << angle << " Speed: " << speed << " CTE: " << cte << " Diff CTE: " << diff_cte << " int CTE: " << int_cte << std::endl;
          std::cout << "pid.Kp: " << pid.Kp <<  " cte: " << cte << " -pid.Kp * cte: " << -pid.Kp * cte << std::endl;
          std::cout << "pid.Kd:" << pid.Kd << " diff_cte: " << " -pid.Kd * diff_cte: " << -pid.Kd * diff_cte << std::endl;
          std::cout << "pid.Ki:" << pid.Ki << " int_cte: " << int_cte << " -pid.Ki * cte: " << -pid.Ki * int_cte << std::endl;

          // Sanitize: Don't let steer_value go out of bound
          if (steer_value < -1) steer_value = -1;
          if (steer_value > 1) steer_value = 1;
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3; // Keep the throttle at 0.3
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
