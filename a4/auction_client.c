#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define BUF_SIZE 128

#define MAX_AUCTIONS 5
#ifndef VERBOSE
#define VERBOSE 0
#endif

#define ADD 0
#define SHOW 1
#define BID 2
#define QUIT 3

/* Auction struct - this is different than the struct in the server program
 */
struct auction_data {
    int sock_fd;
    char item[BUF_SIZE];
    int current_bid;
};

/* Displays the command options available for the user.
 * The user will type these commands on stdin.
 */

void print_menu() {
    printf("The following operations are available:\n");
    printf("    show\n");
    printf("    add <server address> <port number>\n");
    printf("    bid <item index> <bid value>\n");
    printf("    quit\n");
}

/* Prompt the user for the next command 
 */
void print_prompt() {
    printf("Enter new command: ");
    fflush(stdout);
}


/* Unpack buf which contains the input entered by the user.
 * Return the command that is found as the first word in the line, or -1
 * for an invalid command.
 * If the command has arguments (add and bid), then copy these values to
 * arg1 and arg2.
 */
int parse_command(char *buf, int size, char *arg1, char *arg2) {
    int result = -1;
    char *ptr = NULL;
    if(strncmp(buf, "show", strlen("show")) == 0) {
        return SHOW;
    } else if(strncmp(buf, "quit", strlen("quit")) == 0) {
        return QUIT;
    } else if(strncmp(buf, "add", strlen("add")) == 0) {
        result = ADD;
    } else if(strncmp(buf, "bid", strlen("bid")) == 0) {
        result = BID;
    } 
    ptr = strtok(buf, " "); // first word in buf

    ptr = strtok(NULL, " "); // second word in buf
    if(ptr != NULL) {
        strncpy(arg1, ptr, BUF_SIZE);
    } else {
        return -1;
    }
    ptr = strtok(NULL, " "); // third word in buf
    if(ptr != NULL) {
        strncpy(arg2, ptr, BUF_SIZE);
        return result;
    } else {
        return -1;
    }
    return -1;
}

/* Connect to a server given a hostname and port number.
 * Return the socket for this server
 */
int add_server(char *hostname, int port) {
        // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }
    
    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    struct addrinfo *ai;
    
    /* this call declares memory and populates ailist */
    if(getaddrinfo(hostname, NULL, NULL, &ai) != 0) {
        close(sock_fd);
        return -1;
    }
    /* we only make use of the first element in the list */
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;

    // free the memory that was allocated by getaddrinfo for this list
    freeaddrinfo(ai);

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        return -1;
    }
    if(VERBOSE){
        fprintf(stderr, "\nDebug: New server connected on socket %d.  Awaiting item\n", sock_fd);
    }
    return sock_fd;
}
/* ========================= Add helper functions below ========================
 * Please add helper functions below to make it easier for the TAs to find the 
 * work that you have done.  Helper functions that you need to complete are also
 * given below.
 */

/* Print to standard output information about the auction
 */
void print_auctions(struct auction_data *a, int size) {
    printf("Current Auctions:\n");

    /* TODO Print the auction data for each currently connected 
     * server.  Use the follosing format string:
     *     "(%d) %s bid = %d\n", index, item, current bid
     * The array may have some elements where the auction has closed and
     * should not be printed.
     */
    for (int i = 0; i < size; i++){
        if (a[i].sock_fd < 0){
            continue;
        }
        printf("(%d) %s bid = %d\n", i, a[i].item, a[i].current_bid);
    }
}

/* Process the input that was sent from the auction server at a[index].
 * If it is the first message from the server, then copy the item name
 * to the item field.  (Note that an item cannot have a space character in it.)
 */
void update_auction(char *buf, int size, struct auction_data *a, int index) {
    
    // TODO: Complete this function
    char item[size];
    int new_bid;
    long time;
    if (sscanf(buf, "%s %d %ld", item, &new_bid, &time) != 3){
        fprintf(stderr, "ERROE malformed bid: %s\n", buf);
    }
    item[size - 1] = '\0';
    a[index].current_bid = new_bid;
    if (a[index].item[0] == '\0'){
        strncpy(a[index].item, item, BUF_SIZE);
        a[index].item[BUF_SIZE - 1] = '\0';
    }
    printf("\nNew bid for %s [%d] is %d (%ld second left)\n", item, index, new_bid, time);
    

    
    // fprintf(stderr, "ERROR malformed bid: %s", buf);
    // printf("\nNew bid for %s [%d] is %d (%d seconds left)\n",           );
}

int main(void) {

    char name[BUF_SIZE];

    // Declare and initialize necessary variables
    // TODO
    int curr_connect = 0;
    struct auction_data connected[MAX_AUCTIONS];
    for (int i = 0; i < MAX_AUCTIONS; i++){
        connected[i].sock_fd = -1;
        connected[i].current_bid = -1;
        connected[i].item[0] = '\0';
    }
    fd_set fds_set;
    int max_fd = STDIN_FILENO + 1;
    FD_ZERO(&fds_set);

    // Get the user to provide a name.
    printf("Please enter a username: ");
    fflush(stdout);
    int num_read = read(STDIN_FILENO, name, BUF_SIZE);
    if(num_read <= 0){
        fprintf(stderr, "ERROR: read from stdin failed\n");
        exit(1);
    }
    print_menu();
    // TODO
    FD_SET(STDIN_FILENO, &fds_set);
    char todo[BUF_SIZE];
    while(1) {
        print_prompt();
        // TODO
        fd_set listen_fds = fds_set;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("client: select");
            exit(1);
        }
        if (FD_ISSET(STDIN_FILENO, &listen_fds) == 1){
            int num_r = read(STDIN_FILENO, todo, BUF_SIZE);
            if(num_r <= 0){
                fprintf(stderr, "ERROR: read from stdin failed\n");
                exit(1);
            }
            char arg1[BUF_SIZE];
            char arg2[BUF_SIZE];
            todo[num_r] = '\0';
            int comm = parse_command(todo, num_r, arg1, arg2);
            if (comm == ADD){
                if (curr_connect == MAX_AUCTIONS){
                    fprintf(stdout, "Reached max number of auction\n");
                }
                //find index where we insert fd.
                int index = -1;
                for (int i = 0; i < MAX_AUCTIONS; i++){
                    if (connected[i].sock_fd < 0){
                        index = i;
                        break;
                    }
                }
                int port_number = atoi(arg2);
                connected[index].sock_fd = add_server(arg1, port_number);
                if (connected[index].sock_fd == -1){
                    fprintf(stdout, "Connection fail\n");
                }
                else{
                    if (write(connected[index].sock_fd, name, strlen(name)+1) < strlen(name)+1){
                        fprintf(stderr, "write error");
                        exit(1);
                    }
                    FD_SET(connected[index].sock_fd, &fds_set);
                    curr_connect += 1;
                    if(connected[index].sock_fd > max_fd){
                        max_fd = connected[index].sock_fd;
                    }
                }
            
            }
            else if(comm == BID){
                int iteam_index = atoi(arg1);
                if(connected[iteam_index].sock_fd == -1){
                    fprintf(stdout, "Iteam does not exist\n");
                }
                if (write(connected[iteam_index].sock_fd, arg2, strlen(arg2)+1) < strlen(arg2)+1){
                    perror("write error");
                    exit(1);
                }
            }
            else if(comm == SHOW){
                print_auctions(connected, MAX_AUCTIONS);
            }
            else if(comm == QUIT){
                for (int i = 0; i < MAX_AUCTIONS; i++){
                    if (connected[i].sock_fd != -1){
                        close(connected[i].sock_fd);
                    }
                }
                return 0;
            }
            else{
                print_menu();
            }
        }
        //get message from server.
        else {
            for (int i =0; i < MAX_AUCTIONS; i++){
                if ((connected[i].sock_fd != -1) && (FD_ISSET(connected[i].sock_fd, &listen_fds))){
                    char buf[BUF_SIZE];
                    int num_read = read(connected[i].sock_fd, buf, BUF_SIZE);
                    buf[num_read] = '\0';

                    if (num_read == 0){
                        if (close(connected[i].sock_fd) == -1){
                            perror("ERROR: fail to cloase socket\n");
                            exit(1);
                        }
                        FD_CLR(connected[i].sock_fd, &fds_set);
                        connected[i].sock_fd = -1;
                        connected[i].item[0] = '\0';
                        connected[i].current_bid = -1;
                        curr_connect -= 1;
                    }
        
                    if (strncmp(buf, "Auction closed:", strlen("Auction closed:")) == 0){
                        printf("%s\n", buf);
                        if (close(connected[i].sock_fd) == -1){
                            perror("ERROR: fail to cloase socket\n");
                            exit(1);
                        }
                        FD_CLR(connected[i].sock_fd, &fds_set);
                        connected[i].sock_fd = -1;
                        connected[i].item[0] = '\0';
                        connected[i].current_bid = -1;
                        curr_connect -= 1;
                    }
                    else{
                        update_auction(buf, BUF_SIZE, connected, i);
                    }
                
                }
            }
        }    
    }
    return 0; // Shoud never get here
}
