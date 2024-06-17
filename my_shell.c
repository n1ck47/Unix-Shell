#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int bgGid=-1;

char **tokenize(char *line, int *isBackground)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

	    char readChar = line[i];

	    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
		      token[tokenIndex] = '\0';
		      if (tokenIndex != 0){
							tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
							strcpy(tokens[tokenNo++], token);
							tokenIndex = 0; 
		      }
	    } 
	    else {
	      	token[tokenIndex++] = readChar;
	    }
  }
 
  free(token);
  if(tokenNo>0 && strcmp(tokens[tokenNo-1],"&")==0){
  	tokens[tokenNo-1]=NULL;
  	*isBackground=1;
  }
  else{
  	tokens[tokenNo] = NULL ;
  }
  return tokens;
}


void ctrC(int num){
}



int main(int argc, char* argv[]) {
	signal(SIGINT,ctrC);
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;      
	int i;


	while(1) {			
		/* BEGIN: TAKING INPUT */

	
		bzero(line, sizeof(line));
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		int isBackground=0;
		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line, &isBackground);


		int temp = 0;
		if(bgGid>0)
			temp=waitpid(-bgGid,NULL,WNOHANG);
	   	if(temp>0){
				printf("Shell: Background process finished\n");
		  }

	   if(tokens[0]==NULL){
	   	continue;
	   }

   if(strcmp(tokens[0],"cd")==0){
   	 if(chdir(tokens[1])==-1){
   	 	printf("Shell: Incorrect command\n");
   	 }
   	 continue;
   }

   if(strcmp(tokens[0],"exit")==0){
	   	for(i=0;tokens[i]!=NULL;i++){
				free(tokens[i]);
			}
			free(tokens);
			if(bgGid>0)
				kill(-bgGid,SIGTERM);
			kill(0,SIGTERM); // kill every process having groupId equal to that of calling process Id.
   	 _exit(1);
   }

   int pId = fork();

   if(pId==0){
   	 int x = execvp(tokens[0], tokens);
   	 printf("Command doesn't exist\n");
   	_exit(1);
   }
   else if(pId > 0){
   	if(isBackground==0){
   		waitpid(pId);
   	}
   	else{
   		if(setpgid(pId,bgGid)==-1){
   			bgGid=pId;
   		}
   		setpgid(pId,bgGid);
   	}
   }
   else{
   	printf("Unable to create child process\n");
   }
    
       
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
			free(tokens[i]);
		}
		free(tokens);

	}
	return 0;
}
