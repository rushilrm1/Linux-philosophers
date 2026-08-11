/*
 * Dining Philosophers Problem
 * Implementation using fork() and System V Semaphores
 *
 * Compile:  gcc dining_philosophers_detailed.c -o dining_philosophers_detailed
 * Run:      ./dining_philosophers_detailed
 *
 * Idea:
 *  - 5 philosophers  -> 5 child processes
 *  - 5 forks         -> 5 semaphores (one semaphore set of size 5)
 *  - Each fork semaphore starts at value 1 (available)
 *  - To avoid deadlock: even numbered philosophers pick LEFT fork first,
 *    odd numbered philosophers pick RIGHT fork first.
 */

#include"header.h"

#define NUM_PHILOSOPHERS 5
#define NUM_CYCLES 5   /* how many times each philosopher eats */

int semid; /* semaphore set id, shared conceptually (each child gets its own copy after fork,
              but since semaphores live in the kernel, all processes talk to the same set) */

/* wait (P) operation on semaphore number sem_num */
void sem_wait_op(int sem_num)
{
        struct sembuf op;
        op.sem_num = sem_num;
        op.sem_op = -1;   /* decrement -> acquire */
        op.sem_flg = 0;

        if (semop(semid, &op, 1) == -1)
        {
                perror("semop wait failed");
                exit(1);
        }
}

/* signal (V) operation on semaphore number sem_num */
void sem_signal_op(int sem_num)
{
        struct sembuf op;
        op.sem_num = sem_num;
        op.sem_op = 1;    /* increment -> release */
        op.sem_flg = 0;

        if (semop(semid, &op, 1) == -1)
        {
                perror("semop signal failed");
                exit(1);
        }
}

/* the function each philosopher (child process) runs */
void philosopher(int id)
{
        int left_fork = id;
        int right_fork = (id + 1) % NUM_PHILOSOPHERS;
        int i;

        for (i = 0; i < NUM_CYCLES; i++)
        {

                /* THINKING */
                printf("Philosopher %d is thinking...\n", id);
                fflush(stdout);
                sleep(1);

                printf("Philosopher %d is hungry.\n", id);
                fflush(stdout);

                /* deadlock avoidance: even picks left then right, odd picks right then left */
                if (id % 2 == 0)
                {
                        sem_wait_op(left_fork);
                        printf("Philosopher %d picked up left fork %d\n", id, left_fork);
                        sem_wait_op(right_fork);
                        printf("Philosopher %d picked up right fork %d\n", id, right_fork);
                }
                else
                {
                        sem_wait_op(right_fork);
                        printf("Philosopher %d picked up right fork %d\n", id, right_fork);
                        sem_wait_op(left_fork);
                        printf("Philosopher %d picked up left fork %d\n", id, left_fork);
                }

                /* EATING */
                printf("Philosopher %d is eating...\n", id);
                fflush(stdout);
                sleep(1);

                /* release both forks */
                sem_signal_op(left_fork);
                sem_signal_op(right_fork);
                printf("Philosopher %d put down both forks.\n", id);
                fflush(stdout);
        }

        printf("Philosopher %d has finished eating %d times and is leaving.\n", id, NUM_CYCLES);
        exit(0);
}

int main()
{
        key_t key;
        pid_t pid;
        int i;
        union semun
        {
                int val;
                struct semid_ds *buf;
                unsigned short *array;
        } arg;

        /* Step 1: create a key for the semaphore set */
        key = ftok(".", 'S');
        if (key == -1)
        {
                perror("ftok failed");
                exit(1);
        }

        /* Step 2: create semaphore set of 5 semaphores (one per fork) */
        semid = semget(key, NUM_PHILOSOPHERS, IPC_CREAT | 0664);
        if (semid == -1)
        {
                perror("semget failed");
                exit(1);
        }

        /* Step 3: initialize each semaphore to 1 (fork available) */
        for (i = 0; i < NUM_PHILOSOPHERS; i++)
        {
                arg.val = 1;
                if (semctl(semid, i, SETVAL,arg) == -1) {
                        perror("semctl SETVAL failed");
                        exit(1);
                }
        }

        printf("Dining Philosophers started with %d philosophers.\n", NUM_PHILOSOPHERS);

        /* Step 4: create 5 child processes */
        for (i = 0; i < NUM_PHILOSOPHERS; i++)
        {
                pid = fork();

                if (pid < 0)
                {
                        perror("fork failed");
                        exit(1);
                }

                if (pid == 0)
                {
                        /* child process becomes a philosopher */
                        philosopher(i);
                        /* philosopher() calls exit(), so we never reach here */
                }
                /* parent continues the loop to create the next child */
        }

        /* Step 5: parent waits for all children to finish */
        for (i = 0; i < NUM_PHILOSOPHERS; i++)
        {
                wait(NULL);
        }

        printf("All philosophers have finished. Removing semaphore set.\n");

        /* Step 6: remove the semaphore set */
        if (semctl(semid, 0, IPC_RMID) == -1)
        {
                perror("semctl IPC_RMID failed");
                exit(1);
        }

        return 0;
}
