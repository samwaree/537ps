#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#define UIDFIELD 6
#define BUFFSIZE 50

typedef struct Procnode{
        char *pid;
        struct Procnode* next;
}Procnode;     

Procnode* getproclist(){
        DIR *procs;
        DIR *subdir;
        struct dirent *proc_info;
        char *proc = "/proc/";
        char *procdir;
        int uid = getuid();
        FILE *uid_find;
        int uid_line;
        char buff[BUFFSIZE];
        char *filename;
        Procnode *head = (Procnode*) malloc(sizeof(Procnode));
        Procnode *current = head;

        if ((procs = opendir(proc)) == NULL) {
            perror("Couldn't open /procs");
            return NULL;
        }

        // Runs a while loop through all the processes as long as it can access them
        do {
            errno = 0;
            if ((proc_info = readdir(procs)) != NULL) {
                printf("Start of while loop\n");
                procdir = (char*) malloc(1 + sizeof(proc) + sizeof(proc_info->d_name));
                strcpy(procdir, proc);
                strcat(procdir, proc_info->d_name);
                
                printf("%s\n", procdir);
                // Accesses the directory as long as it is not null and not a string
                if((atoi(proc_info->d_name) != 0) && (subdir = opendir(procdir)) != NULL){
                        uid_line = 0;
                        filename = (char*) malloc(sizeof(procdir) + sizeof(char)*5);
                        strcpy(filename, procdir);
                        strcat(filename, "/status");
                        
                        printf("%s\n", filename);    
                        // Attempts to open the status file within process directory
                        uid_find = fopen(filename, "r");
                        if(uid_find != NULL){
                                while(uid_line < 9){
                                        fgets(buff, BUFFSIZE, uid_find);
                                        uid_line++;
                                }
                                fclose(uid_find);
                                                     
                                                         // Get the UID from the status file
                                char *uidcomp;
                                char *charptr = buff;
                                uid_line = 0;

                                // Parses the line to retrieve the UID associated with the process
                                while((uidcomp = strtok_r(charptr, "\t", &charptr)) != NULL && uid_line < 2){
                                        // if uid and pid match, return the PID
                                        if(uid == atoi(uidcomp)){
                                                printf("Success. %s was accessed.\n", proc_info->d_name);

                                                // Add the PID to an array to return
                                                current->pid = malloc(1 + sizeof(proc_info->d_name));
                                                strcpy(current->pid, proc_info->d_name);
                                                printf("%s\n", current->pid);
                                                current->next = malloc(sizeof(Procnode));
                                                current = current->next;
                                                current->pid = NULL;
                                        }
                                        uid_line++;
                                        
                                }
                        }
                        
                        // Frees the file pointer and close the subdirectory
                        free(filename);
                        if(closedir(subdir) == -1){
                                perror("closedir");
                                return NULL;
                        }
                
                // Frees the process directory pointer
                free(procdir);
                
                }
            }
        } while (proc_info != NULL);

        // Closes /proc
        if(closedir(procs) == -1){
                perror("closedir");
                return NULL;
        }
        printf("End of getprocs");
        return head;
}


int getproc(char* pid){
        DIR *procs;
        DIR *subdir;
        struct dirent *proc_info;
        char *proc = "/proc/";
        char *procdir;
        int uid = getuid();
        FILE *uid_find;
        int uid_line;
        char buff[BUFFSIZE];
        char *filename;
        procs = opendir(proc);

        // Runs a while loop through all the processes as long as it can access them
        while((proc_info = readdir(procs)) != NULL){
            if(atoi(pid) == atoi(proc_info->d_name)){
                procdir = (char*) malloc(1 + sizeof(proc) + sizeof(proc_info->d_name));
                strcpy(procdir, proc);
                strcat(procdir, proc_info->d_name);

                // Accesses the directory as long as it is not null
                if((subdir = opendir(procdir)) != NULL){
                        uid_line = 0;
                        filename = (char*) malloc(sizeof(procdir) + sizeof(char)*5);
                        strcpy(filename, procdir);
                        strcat(filename, "/status");

                        // Attempts to open the status file within process directory
                        uid_find = fopen(filename, "r");
                        if(uid_find != NULL){
                                while(uid_line < 9){
                                        fgets(buff, BUFFSIZE, uid_find);
                                        uid_line++;
                                }
                                fclose(uid_find);
                                                     
                                                         // Get the UID from the status file
                                char *uidcomp;
                                char *charptr = buff;
                                uid_line = 0;

                                // Parses the line to retrieve the UID associated with the process
                                while((uidcomp = strtok_r(charptr, "\t", &charptr)) != NULL && uid_line < 2){
                                        // if uid and pid match, return 1
                                        if(uid == atoi(uidcomp)){
                                                //printf("\nSuccess. %s was accessed.", proc_info->d_name);
                                                free(filename);
                                                if(closedir(subdir) == -1){
                                                        perror("closedir");
                                                        return 0;
                                                }
                                                // Frees the process directory pointer
                                                free(procdir);
                                                // Closes /proc
                                                if(closedir(procs) == -1){
                                                       perror("closedir");
                                                       return 0;
                                                }
                                                return 1;
                                             
                                                
                                        }
                                        
                                        // For single PID lookup, if UID does not match, return 0
                                        if(uid_line == 1 && uid != atoi(uidcomp)){
                                                free(filename);
                                                if(closedir(subdir) == -1){
                                                        perror("closedir");
                                                        return 0;
                                                }
                                                // Frees the process directory pointer
                                                free(procdir);
                                                // Closes /proc
                                                if(closedir(procs) == -1){
                                                       perror("closedir");
                                                       return 0;
                                                }
                                                return 0;
                                        }

                                        uid_line++;
                                }
                        }

                        // Frees the file pointer and close the subdirectory
                        free(filename);
                        if(closedir(subdir) == -1){
                                perror("closedir");
                                return 0;
                        }

                // Frees the process directory pointer
                free(procdir);
                }
            }
        }
        if(closedir(procs) == -1){
             perror("closedir");
             return 0;
        }
        return 0;
}

