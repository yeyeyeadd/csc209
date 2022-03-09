#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

Q6. [9 marks] 
"Consider a program similar to assignment 3 where a process will fork child
processes to carry out parts of the work.  In this case, each time a child 
completes a chunk of work, it will write one integer indicating its result to 
a pipe connected to the parent process. The parent will add this result to
"sum". It is not known how many times each child will write to the parent.

Your task is to implement the part of the program where the parent process
reads all the results from the pipes connected to the child processes.
Use select in the loop below so that the parent process will not block on
any read call. 
If the child closes their end of the pipe, the parent should handle that 
case appropriately.  (Error checking is not required.)

The function "is_empty(fd_set *fds)" will return true if the set is empty.  
You do not need to write this function. "

int num_procs;  // the number of child processes
fds[num_procs][2];  // assume these descriptors are open correctly
fd_set allfds;
FD_ZERO(&allfds);
int maxfd = 0;
int sum = 0;

// TODO initialization if needed
for (int i = 0; i < num_procs; I++){
    close(fds[i][1]);
    FD_SET(fds[i][0], &allfds);
    if (fds[i][0] > maxfd){
        maxfd = fds[i][0]
    }
}

// Read from the pipes connected to the child processes
while(!is_empty(&allfds)) {
    // TODO 
    fd_set listen_fds = allfds;
    int neardy = select(maxfd+1, &listen_fds, NULL, NULL, NULL);
    for (int i = 0; i < num_procs; I++){
        int value = 0;
        if (FD_ISSET(fds[i][0], &listen_fds)){
            int number_read = read(fds[i][0], &value, sizeof(int));
        }
        if (number_read == 0){
            close(fds[i][0]);
            FD_CLR(fds[i][0], &allfds);
        }
        sum += num;
    }
}
    
    
}

printf("Sum is %d\n", sum);
