
#include "ThreadManager.hpp"
#include "Helper.hpp"

#include <pthread.h>
#include <sys/socket.h>
#include <iostream>
#include <semaphore.h>
#include "stdio.h"
#include "stdlib.h"
#include <sys/wait.h>
#include <errno.h>
#include <set>
#include <map>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include "stdlib.h"

using namespace MY_THREADS;
using namespace std;

int ThreadManager::totalThreadsCreated = 0;
int ThreadManager::totalProcessCreated = 0;

int ThreadManager::readThreadsCounter = 0;
int ThreadManager::writeThreadsCounter = 0;
int ThreadManager::maximumThreads = 20; // updated by configuration

// for client connections
int ThreadManager::clientListenThreadsCounter = 0;

// Peer to peer communication thread management
int ThreadManager::peerConnectThreadsCounter = 0;
int ThreadManager::peerListenThreadsCounter = 0;

// sem_t ThreadManager::read_sem;
sem_t ThreadManager::write_sem;
sem_t ThreadManager::comm_pipe_sem;
pthread_mutex_t ThreadManager::mutex_read_wrt;

std::vector<int> ThreadManager::threads = {};
std::vector<int> ThreadManager::processes = {};
pipesFd ThreadManager::commPipe;

void *(ThreadManager::*createProcessPtrFunc)(void *) const = NULL;

// Configure semaphore

ThreadManager::ThreadManager()
{
    // sem_init(&ThreadManager::read_sem, 0, 1);
    // sem_init(&ThreadManager::write_sem, 0, 1);
    // sem_init(&ThreadManager::update_sem, 0, 1);
}

ThreadManager::~ThreadManager()
{
    pthread_mutex_destroy(&mutex_read_wrt);
    sem_destroy(&write_sem);
    sem_destroy(&comm_pipe_sem);
}

void ThreadManager::init()
{
    totalThreadsCreated = 0;

    // Configure semaphore
    // sem_init(&read_sem, 0, 1);
    sem_init(&write_sem, 0, 1);
    sem_init(&comm_pipe_sem, 0, 1);
    pthread_mutex_init(&ThreadManager::mutex_read_wrt, NULL);

    createPipe(&commPipe);
    // cout << "Communication pipe" << endl;
    // cout << commPipe.mainReadFd << endl;
}

int ThreadManager::createThread(void *functionToCall(void *), void *argToFuctionToCall)
{
    BotManager::printf_debug("Create thread\n");

    unsigned long tid_out; // temporary hold the thread id created
    // check if there is resource to create thread.
    if ((totalThreadsCreated + totalProcessCreated) >= maximumThreads)
    {
        perror(("CreateThread: No available resource to create more threads: " +
                to_string((totalThreadsCreated + totalProcessCreated)))
                   .c_str());

        return -1;
    }
    errno = 0;

    if (pthread_create(&tid_out, NULL, functionToCall, argToFuctionToCall) != 0)
    {
        perror(" CreateThread ");
        return -1;
    }

    totalThreadsCreated++;
    try
    {
        /* code */
        threads.push_back(tid_out);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    BotManager::printf_debug("Thread created:  " + to_string(tid_out));

    return tid_out;
}

// kills the current thread that is called,  if an id is not specified
int ThreadManager::killThread(unsigned long tid, void **returnValue)
{
    errno = 0;
    if (tid == -1)
    {
        tid = pthread_self();
    }
    if (pthread_join(tid, returnValue) != 0)
    {
        perror("KillThread: ");
        pthread_exit(0);
        return -1;
    }
    if (totalThreadsCreated > 0)
        totalThreadsCreated--;
    return 0;
}

void ThreadManager::killAll(bool killLeader, int signal, bool skipBasePid)
{
    BotManager::printf_debug("kill-all called");

    // load processes from file
    ifstream fs;
    int base_pid = -1;
    int grandPid = 0;
    try
    {
        fs.open("bbserv.pid", ios::in);

        if (fs.is_open())
        {
            string line;
            getline(fs, line);
            fs.close();

            if (line.length() > 0)
            {
                base_pid = stoi(line);
                grandPid = base_pid;
                base_pid = ((int)(base_pid / 100)) * 100;
            }
        }
        processes = {};
        BotManager::printf_debug("Base process: " + to_string(base_pid));

        int failedCount = 98;
        while (failedCount > 0)
        {
            if (skipBasePid && grandPid <= base_pid)
            {
                // pass
            }
            else if (kill(base_pid, 0) == 0)
            { // don't kill just check if exist
                processes.push_back(base_pid);
                BotManager::printf_debug("Reaper found process to kill: " + to_string(base_pid));
            }
            base_pid++;
            failedCount--;
        }

        BotManager::printf_debug("Processes: " + to_string(processes.size()));

        if (processes.size() > 0)
        {
            int i = processes.size() - 1;
            while (i > 0)
            {
                try
                {
                    if (skipBasePid && grandPid <= processes[i])
                    {
                        // pass
                    }
                    else
                    {
                        killProcess(processes[i], signal);
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }
                i--;
            }
        }

        // reset containers
        threads = {};
        processes = {};
    }
    catch (exception &e)
    {
        std::cerr << e.what() << endl;
        std::cerr << "Error reading base process id pid file" << endl;
        fs.close();
        return;
    }
}

int ThreadManager::createProcess(void *functionToCall(void *), void *argsToFuctionToCall)
{
    //    printf_debug( "Entered creating process" );



    // create child process that should be replaced by the calling function;
    int status;
    status = fork();
    if (status < 0)
    {
        perror("Failed to create a child(1) processd ");
        return -1;
    }
    else if (status == 0)
    {

        // dispatch procedure
        (*functionToCall)(argsToFuctionToCall);

        // exit(EXIT_SUCCESS);
        return 0;
    }
    // printf("PID: %d", status);


    return status;
}

int ThreadManager::killProcess(unsigned long pid, int signal)
{
    errno = 0;

    int done = -1;
    try
    {
        // dispatch terminate and wait for S seconds.
        // if not terminated, kill
        if (kill(pid, signal) == 0)
        {
            BotManager::printf_debug("Termination successfull");
            done = 0;
        }
        else
        {
            std::cerr << "Process: " << pid << " termation failed!" << endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    if (totalProcessCreated > 0)
        --totalProcessCreated;
    return done;
}

pipesFd *ThreadManager::createPipe(pipesFd *pipes, takeOverStdio io)
{
    //  pipesFd *pipes = (pipesFd*)malloc(sizeof(pipesFd));
    errno = 0;

    BotManager::printf_debug("Entered createPipe: ");
    try
    {
        int fd[2];
        if (pipe2(fd, O_NONBLOCK) == -1)
        {
            perror("CreatePipe: ");
            return pipes;
        }

        // printf_debug("Entered createPipe2: ");
        pipes->mainReadFd = fd[0];  // store the main pipe.
        pipes->mainWriteFd = fd[1]; // store the main pipe.
        int t = pipes->mainReadFd;
        // printf_debug((to_string(t).c_str()));

        // printf_debug("Entered createPipe3: ");

        if (io == takeOverSTDERR)
        {

            int stdErrFd[2];
            if (pipe(stdErrFd) == -1)
            {
                perror("CreatePipe: TakeOver stdio failed: ");
                // return NULL;
            }
            BotManager::printf_debug("Entered createPipe stderr: ");

            if ((dup2(STDERR_FILENO, stdErrFd[1]) == -1) ||
                (dup2(fd[1], STDERR_FILENO) == -1))
            {
                perror("CreatePipe: Taking over standard error failed: ");
                // close(stdErrFd[1]);
                // return NULL;
            };

            close(fd[1]); // free discriptor, since it has been taken over by the stdout
            BotManager::printf_debug("Successfully taken over standard error : ");
            pipes->stdReadFd = stdErrFd[0];
            pipes->stdWriteFd = stdErrFd[1];
        }

        if (io == takeOverSTDIN)
        {
            BotManager::printf_debug("Entered createPipe stdin: ");
            int stdInFd[2];
            if (pipe(stdInFd) == -1)
            {
                perror("CreatePipe: TakeOver stdio failed: ");
                // return NULL;
            }

            if ((dup2(STDIN_FILENO, stdInFd[0]) == -1) ||
                (dup2(fd[0], STDIN_FILENO) == -1))
            {
                perror("CreatePipe: Taking over standard input failed: ");
                // close(stdInFd[0]);
                // return NULL;
            }

            close(fd[0]); // free discriptor, since it has been taken over by stdin
            BotManager::printf_debug("Successfully taken over standard input: ");
            pipes->stdReadFd = stdInFd[0];
            pipes->stdWriteFd = stdInFd[1];
        }

        if (io == takeOverSTDOUT)
        {

            std::cerr << ("Entered createPipe stdout: \n");
            int stdOutFd[2];
            if (pipe(stdOutFd) == -1)
            {
                perror("CreatePipe: TakeOver stdio failed: ");
                // return NULL;
            }

            if ((dup2(STDOUT_FILENO, stdOutFd[1]) == -1) ||
                (dup2(fd[1], STDOUT_FILENO) == -1))
            {
                perror("CreatePipe: Taking over standard output failed: ");
                // close(stdOutFd[1]);
                // return NULL;
            };

            close(fd[1]); // free discriptor, since it has been taken over by the stdout
            std::cerr << ("Successfully taken over standard output : \n");
            pipes->stdReadFd = stdOutFd[0];
            pipes->stdWriteFd = stdOutFd[1];
        }

        // return fd;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        // return NULL;
    }
    return pipes;
}

int ThreadManager::killPipe(pipesFd pip, takeOverStdio io)
{

    int error = 0;
    // perror((string("KillPipe: PipeRef: ") + to_string(pip.mainReadFd)).c_str());
    try
    {
        if (io == takeOverSTDIN)
        {

            if (dup2(STDIN_FILENO, (pip.mainReadFd)) == -1 ||
                dup2((pip.stdReadFd), STDIN_FILENO) == -1)
            { // de-reference to 0
                perror("KillPipe: retore Takover stdin failed: ");
                error = -1;
            }
        }
        if (io == takeOverSTDOUT)
        {

            if (dup2(STDOUT_FILENO, (pip.mainWriteFd)) == -1 ||
                dup2((pip.stdWriteFd), STDOUT_FILENO) == -1)
            { // de-reference to 1
                perror("KillPipe: retore Takover stdout failed: ");
                error = -1;
            }

            BotManager::printf_debug("Done restoring pipe");
        }

        if (io == takeOverSTDERR)
        {

            if (dup2(STDERR_FILENO, (pip.mainWriteFd)) == -1 ||
                dup2((pip.stdWriteFd), STDERR_FILENO) == -1)
            { // de-reference to 1
                perror("KillPipe: retore Takover stderr failed: ");
                error = -1;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    try
    {
        close(pip.stdReadFd);
        close(pip.stdWriteFd);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    //
    try
    {
        close(pip.mainReadFd);
        close(pip.mainWriteFd);
    }
    catch (exception e)
    {
    }

    ThreadManager::sleep_(2);

    return error;
}

// custom sleep function
int ThreadManager::sleep_(long seconds)
{
    long milisec = seconds * 1000;
    struct timespec rem;
    struct timespec req = {
        (long)(milisec / 1000),
        (milisec % 1000) * 1000000};
    return nanosleep(&req, &rem);
}