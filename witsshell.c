#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <ctype.h>
#define PATH "/bin/"

char* currentPath[100];
char* commands[100];
char* allCommands[100];
int numberOfCommands = 0;
int numberOfAllCommands = 0;
int numberOfPaths = 0;
int pathNumber = 0;

void printError(){
	char error_message[30] = "An error has occurred\n"; //The only error message
	write(STDERR_FILENO, error_message, strlen(error_message));

}

void exec(char* commands[]){ 
	int failExecution = 0;
	int successful = fork();
    if (successful== 0){
		
		int i=0;
		int t =0;
		while(currentPath[i]!=0){
			char run[50] = "";
			strcat(run, currentPath[i]);
			int k = numberOfPaths;
			
			strcat(run, commands[0]);
			bool fileExist = (access(run,F_OK)==0); 
			if(fileExist){
				execv(run, commands);
				k++;
			}
				
			else failExecution++;
			i++;
		}
		if(failExecution==numberOfPaths){
			printError();
		}
		exit(0);           
	}else
        wait(NULL); 
}

bool haveAccess(char* command){
	int i=0;
	while(currentPath[i]!=0){
		int k = i;
		char run[50] = "";
		strcat(run, currentPath[i]); 
		strcat(run, "/");
		strcat(run, command); 
		int j = access(run, F_OK);
		if(j==0){
			//if(pathNumber != NULL){
				pathNumber = i;
			//}
			//pathNumber = i;

			return true;
		} 
		i++;
	}
	return false;
}

char* strtrim(char* str) { 
size_t len = strlen(str);
    if (len == 0) {
      return str;
    }
size_t start = 0;
    while (isspace(str[start])) {
      start++;
    }
size_t end = len - 1;
    while (isspace(str[end])) {
      end--;
    }
size_t i;
    for (i = 0; i <= end - start; i++) {
		if(len > 1 && str[end]=='&'){
			str[end]='\0';
		}
			str[i] = str[start + i];
		
      
    }
    str[i] = '\0';
    return str;
}
void storeCommand(char* CommandLine){  
	int i = 0;
	strtrim(CommandLine); //Trim whitespaces in the command line.
	char* tp;
	size_t len = strlen(CommandLine);
	int ends_with_and = 0;
	if(len > 1 && CommandLine[len-1]=='&'){
		CommandLine[len-1]='\0';
		ends_with_and++;
	}
	strtrim(CommandLine); //Trim whitespaces in the command line.
	memset(commands, 0, sizeof(commands));
	char * token = strtok(CommandLine, " ");
	int count =0;
   	while( token != NULL ) {
      	commands[i] = token;
      	token = strtok(NULL, " ");
		count++;
		i++;
   	}
	numberOfCommands = i;
}

int checkForEOF() {
    int c_in = getc(stdin);
    if (c_in == EOF) {
        return 1; // EOF encountered
    }
    ungetc(c_in, stdin);
    return 0; // Not EOF
}

void excecuteCommand(char *Allcommands[]){ 
	for(int i=0;i<numberOfAllCommands;i++){
		storeCommand(Allcommands[i]);

		int is_ls = strcmp(commands[0], "ls"); //done
		int is_exit = strcmp(commands[0], "exit"); //done
		int is_echo = strcmp(commands[0], "echo");//done
		int is_cd = strcmp(commands[0], "cd"); //done
		int is_path = strcmp(commands[0], "path");//done
		int is_and = strcmp(commands[0], "&"); //done
		//int is_and_last = strcmp(commands[numberOfAllCommands-1], "&");
		

		if(is_ls==0){
			if(numberOfCommands>1){
		for(int i=0;i<numberOfCommands;i++){
			if(strcmp(commands[i], ">")==0){
				if(i==numberOfCommands-1 || i<numberOfCommands-1){
					printError();
					return;
				}
			}
		}
		int fileAccess;
		fileAccess = access(commands[1], F_OK);
		if(fileAccess!=0){
			char error_message[100] = "ls: cannot access '";
			strcat(error_message, commands[1]);
			strcat(error_message, "': No such file or directory\n");
			write(STDERR_FILENO, error_message, strlen(error_message));
			//printError();
		}else{
			exec(commands);
		}
	}else{ 
		exec(commands);
	}

		}else if(is_exit==0){
			if(numberOfCommands==1){
				exit(0);
			}else{ 
				printError();
			}
		}else if(is_echo==0){
			if(numberOfCommands!=1){
				char temp[250] ="";
				int coms = 0;
				for(int j=1;j<numberOfCommands-1;j++){
					coms++;
					strcat(temp, commands[j]);
					strcat(temp, " ");
				}
				strcat(temp, commands[coms+1]);
				printf("%s\n", temp);
			}else{
				printf("\n");
			}
		}else if(is_cd==0){
			if(numberOfCommands!=2){
				printError();
			}else{
				int fileAccess;
				fileAccess = access(commands[1], F_OK);
				if(fileAccess!=0){
					printError();
				}else{
					if(chdir(commands[1])!=0){
						printError();
					}
				}
			}
		}else if(is_path==0){
			
			if(numberOfCommands==1){
				int isPath=is_path;
				memset(currentPath, 0, sizeof(currentPath));
				
				numberOfPaths = 0;
				
			}else{
				for(int i=0;i<numberOfCommands-1;i++){
					int isPath = 1;
					currentPath[numberOfPaths+1] = commands[i+1];
					
					numberOfPaths++;
				}
			}
			if(numberOfCommands==1){
		memset(currentPath, 0, sizeof(currentPath));

		numberOfPaths = 0;
	}else{
		int isPath =0;
		isPath=0;
		for(int i=0;i<numberOfCommands-1;i++){
			currentPath[numberOfPaths+1] = commands[i+1];
			isPath+=1;
			numberOfPaths++;
		}
	}
		}else if(haveAccess(commands[0])){
			////////////
			char t_p[50] = "";
			int allowed = 0;
			strcat(t_p, currentPath[pathNumber]);
			strcat(t_p, commands[0]);
			if(strlen(t_p)==2){
				allowed++;
			}
			commands[0] = "sh";
			commands[1] = t_p;

			commands[3] = NULL;
			exec(commands);
		}else if(is_and==0){

			continue;
		}
		else{
			printError();
		}
	}
}
void batchmode(char *MainArgv){
	int i = 0;
	FILE* readFile = fopen(MainArgv, "r");
	int status = 1;
	if(readFile==NULL){
		printError();
		exit(1);
	}else{
		char* contents = NULL;
		size_t len = 0;
		while (getline(&contents, &len, readFile) != -1){
			if(strcspn(contents, "\n")==0){
				//running = running + 1;
				continue;
			}
			contents[strcspn(contents, "\n")] = 0;
			allCommands[i] = strdup(contents);
			i= i+1;
		}
		numberOfAllCommands=i;
		fclose(readFile);
		status = 0;
		free(contents);

		excecuteCommand(allCommands);
	}
}
int main(int MainArgc, char *MainArgv[]){
	int numArg = MainArgc;
	int k = 0;
	int mode =0;
	currentPath[0] = PATH;
	if(MainArgc == 1){
		char *buffer = NULL;
    size_t bufsize = 32;
	while(true){
		printf("witsshell> ");
		if(checkForEOF()){  
			exit(0);
		}
		getline(&buffer,&bufsize,stdin);
		if(strcmp(buffer, "\n")==0){
			continue;
		}
		buffer[strcspn(buffer, "\n")] = 0;
		allCommands[0] = buffer;
		numberOfAllCommands = 1;
		excecuteCommand(allCommands);
	}free(buffer);
	
	}else if(numArg == 2){
		
		int fileAccess = access(MainArgv[0], X_OK);
		
		if(fileAccess==0){
			mode++;

			batchmode(MainArgv[1]);
		}else{
			printError();
			return 1;
		}
	}else{
		printError();
		return 1;
	}
	return 0;
}
