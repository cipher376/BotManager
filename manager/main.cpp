#include "ThreadManager.hpp"
#include <string>
#include "string.h"
#include <wait.h>
#include <pthread.h>
#include <unistd.h>
#include "signal.h"
#include "Helper.hpp"



using namespace std;
using namespace MY_THREADS;
using namespace BotManager;

pid_t childPid = 0;

void signalHandler(int signal)
{
    if (signal == SIGTERM)
    {
        kill(childPid, SIGKILL);
    } else if (signal == SIGINT)
    {
        /* code */
        kill(getpid(), SIGKILL);
        kill(getppid(), SIGKILL);
    }
    
}

void *startBot(void *botUrl)
{
   

    char *argv[3]; // arguments list for execvp
    argv[0] = (char *)malloc(10 * sizeof(char));
    argv[1] = (char *)malloc(7 * sizeof(char));
    const char *url = (char *)botUrl;
    argv[2] = (char *)malloc((200) * sizeof(char));


    strcpy(argv[0], "python3");
    strcpy(argv[1], "bot/src/botController.py");
    strcpy(argv[2], url);
    argv[3] = NULL; // End of argument

    printf("%s,", url);

    errno = 0;

    // // calling execvp now
    int status = fork();
    if (status < 0)
    {
        perror("Fork bot failed");
    }
    else if (status == 0)
    {
        
        
        int statusExec = execvp(argv[0], argv);
        if (statusExec < 0)
        {
            perror("Failed to run execvp call \n");
        }
        exit(EXIT_SUCCESS);
    }
    
    // printf("Process Id: %d",status);
    system("./startTor.sh");

    return NULL;
}


void *refreshBotLink(void *testLink){
    char *argv[3]; // arguments list for execvp
    argv[0] = (char *)malloc(12 * sizeof(char));
    const char *url = (char *)testLink;
    argv[1] = (char *)malloc((200) * sizeof(char));


    strcpy(argv[0], "firefox_tor");
    strcpy(argv[1], url);
    argv[2] = NULL; // End of argument


    errno = 0;

    // // calling execvp now
    int status = fork();
    if (status < 0)
    {
        perror("Fork bot failed");
    }
    else if (status == 0)
    {
        int statusExec = execvp(argv[0], argv);
        if (statusExec < 0)
        {
            perror("Failed to run execvp call \n");
        }
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
    perror("Before exit)");
    return NULL;
}





int main(int argc, char *argv[])
{
    // printf("%s", argv[1]);

    if (signal(SIGTERM, signalHandler) == SIG_ERR)
    {
        printf_debug("Unable to catch termination signal\n");
    }

    start:
    try
    {
        if(childPid>0){
             killpg(getpid(), SIGKILL);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    for (int i = 0; i < 100; i++)
    {
        childPid = ThreadManager::createProcess(&startBot, (void *)"https://check.torproject.org");
        // printf("My pid: %d", pid);
        if(childPid > 0){
            kill(childPid, SIGKILL);
        }
    }
    
    // ThreadManager::createProcess(&refreshBotLink, (void *)"https://check.torproject.org");

    ThreadManager::sleep_(5);

    // ThreadManager::sleep_(10);
    
    // int i = 0;
    // while(true){
    //     ThreadManager::sleep_(10);
    //     i+=10;
    //     if(i >= 120) {
    //         // kill the process
    //         break;
    //     }
    // }
    
    
    childPid = ThreadManager::createProcess(&startBot, argv[3]); 
    printf("PID: %d\n", childPid);
    
    // Randomize process time to live

    long unsigned int minTime = 30;
    long unsigned int maxTime = 240;
    try
    {
        minTime = stoi(argv[1]);
        maxTime = stoi(argv[2]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        minTime = 30;
        maxTime = 240;
    }
    
    Randomer randomer{minTime, maxTime};
    ThreadManager::sleep_(randomer());
    killpg(getpid(), SIGTERM);
    ThreadManager::sleep_(5);


    goto start;
    
    
    exit(EXIT_SUCCESS);

    return 0;
}
