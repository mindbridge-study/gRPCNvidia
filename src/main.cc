#include "Client.h"
#include "Server.h"
#include "ThreadManager.h"
#include "TransferData.h"

#include <mutex>

/*
 * Thread Dependencies
 */
std::mutex mtx;
Data<int, int, int> data(1, 2, 3);

/*
* Thread Creation
*/
ThreadArgument args[2] = {
  {"Server", 
  []() -> bool {
    std::cout << "Server Iteration" << std::endl;
    Server obj;
    obj.Run();
    return true;
  }, 
  []() -> void {
    std::cout << "Server Cleanup" << std::endl;
  }}, 

  {"Client", 
  []() -> bool { 
    std::cout << "Client Iteration" << std::endl;
    Client obj;
    obj.SendImage("Hello World");
    return true;
  }, 
  []() -> void {
    std::cout << "Client Cleanup" << std::endl;
  }}
};


int main(int argc, char* argv[]) {
  // Program Starts running functions
  ThreadManager<2> manager(args);

  // Code Underneath will run as well
  std::cout << "Main Thread" << std::endl;
  return 0;
}
