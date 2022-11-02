
#include <vector>
#include <set>
#include <map>
#include <semaphore.h>
#include <stdlib.h>
#include <signal.h>


#ifndef ThreadManager_hpp
#define ThreadManager_hpp

#define  MAX_CLIENT_SOCKET_THREADS 5
#define  MAX_READ_THREADS 5
#define  MAX_WRITE_THREADS 1

#define  MAX_PEER_CONNECT_THREADS 4
#define  MAX_PEER_LISTEN_THREADS 5


#define SYNCPORT 10000
#define BBPORT 9000
#define MAX_THREAD 20 // TMAX: maximum thread to be created
#define DAEMON 1 // True 
#define DEBUG 0




namespace MY_THREADS
{
    


    struct pipesFd{
        int mainReadFd;
        int mainWriteFd;
        int stdReadFd; // holds the original stdio pipe address
        int stdWriteFd; // holds the original stdio pipe address
    };

      struct processParam{
        // holds the pipe location on the pipes map static member variable
        int readPipeId;
        int writePipId;
        pipesFd commPipe;
    };

   
    enum takeOverStdio {
        takeOverSTDOUT,
        takeOverSTDERR,
        takeOverSTDIN,
        takeOverNONE
    };

    class ThreadManager
    {
    private:
    public:
        static int maximumThreads; // maximum threads available.
        
        static const int max_client_socket_threads = MAX_CLIENT_SOCKET_THREADS;
        static const int max_read_Threads = MAX_READ_THREADS;
        static const int max_write_Threads = MAX_WRITE_THREADS;

        static const int max_peer_connect_threads = MAX_PEER_CONNECT_THREADS;
        static const int max_peer_listen_Threads = MAX_PEER_LISTEN_THREADS;


        static int readThreadsCounter;
        static int clientListenThreadsCounter;
        static int writeThreadsCounter;

        static int peerConnectThreadsCounter;
        static int peerListenThreadsCounter;


        static int totalThreadsCreated;               // total threads to created.
        static int totalProcessCreated;               // total threads to created.
        static std::vector<int> threads;            // hold thread ids
        static std::vector<int> processes;// hold thread ids
        
        static sem_t /*read_sem,*/ write_sem, comm_pipe_sem;
        static pthread_mutex_t mutex_read_wrt;
        static pipesFd commPipe; // high way communication for processes;

        ThreadManager();

        ~ThreadManager();

        static int sleep_( long seconds);


        // Operations
        // Setup threading facilities
        static void init();


        static int createThread(void *functionToCall(void *), void *argsToFuctionToCall);
        // enable communication through pipes
        // takover standard input output if necessary with by turning them on
        // if you enabled comm, then make sure you destroy the pipes in the calling function after usage
        // return pipeRefOnStack if comm is enabled else return child process id,
        // -1 if failed
        static  int createProcess(void *functionToCall(void *), void *argsToFuctionToCall);

        // return 0 on success else -1;
        // You can only take over one io at a time
        // if you take over the standard IO, remember to restore in kill pipe
        // pipes created will be return through the pipes pointer
        static pipesFd *createPipe(pipesFd *pipeFds, takeOverStdio io=takeOverNONE);

        // remember to set appropriate take overs if it was set in its creation
        static int killPipe(pipesFd pip, takeOverStdio io=takeOverNONE);

        // kills the current thread that is called,  if an id is not specified
        static int killThread(unsigned long tid = -1, void **returnValue = NULL);
        static int killProcess(unsigned long tid = -1, int signal=SIGTERM);
        
        static void killAll(bool killLeader=false, int signal=SIGTERM, bool skipBasePid=false);

        static  void * (ThreadManager::*createProcessPtrFunc)(void *) const;


        // Getters
        // sem_t getReadSem()
        // {
        //     return read_sem;
        // }
        // sem_t getWriteSemaphore()
        // {
        //     return write_sem;
        // }
        // sem_t getUpdateSemaphore()
        // {
        //     return update_sem;
        // }

        // and setters


        // helper functions
        static void lockSemaphore(sem_t *sem)
        {
            sem_wait(sem);
        }
        static void unlockSemaphore(sem_t *sem)
        {
            sem_post(sem);
        }
        static void destroySemaphore(sem_t *sem)
        {
            sem_destroy(sem);
        }
        
        
        
    };

} // namespace BBA

#endif