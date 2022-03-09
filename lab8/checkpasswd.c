#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
     Please read the comments in validate carefully.
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  // TODO
  if (strlen(user_id) > MAX_PASSWORD){
    printf(NO_USER);
  }
  if (strlen(password) > MAX_PASSWORD){
    printf(INVALID);
  }


  int status;
  
  int rw[2];
  pipe(rw);
  int n;
  n = fork();
  // for parent
  if (n > 0){
    close(rw[0]);
    write(rw[1], user_id, 10);
    write(rw[1], password, 10);
    close(rw[1]);

    if(wait(&status) == -1){
      perror("wait");
    }
    else {
      if(WIFEXITED(status)){
        if( WEXITSTATUS(status) == 0){
          printf(SUCCESS);
        }
        else if ( WEXITSTATUS(status) == 2 ){
          printf(INVALID);

        }
        else if ( WEXITSTATUS(status) == 3 ){
          printf(NO_USER);

        }
      }
    }  
  }

  // for child 
  else if ( n == 0 ) {
    // close the write pipe
    close(rw[1]);
    
    if (dup2(rw[0], fileno(stdin)) == -1) {                                     
        perror("dup2");                                                           
    }

    close(rw[0]);                                                        
    execl("./validate", "validate", NULL);                    
    perror("execl");                                                              
    exit(1);                                                                      
    }

  else {
    perror("fork");
    exit(1);
  }

  return 0;
}
