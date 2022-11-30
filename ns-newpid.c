define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/utsname.h>

static char child_stack[1048576];
static int env_var_buf_size = 100;

static void print_nodename() {
  struct utsname utsname;
  uname(&utsname);
  printf("%s\n", utsname.nodename);
}


static int child_fn() {


  printf("PID: %ld\n", (long)getpid());
  __int32_t parent_pid = getppid();
  if(parent_pid == 0) {
    printf("Child PID has no parent (Parent PID: %ld)\n", (long)parent_pid);
  } else {
    printf("Parent PID: %ld\n", (long)parent_pid);
  }
}
static int child_fn_namespace () {
  child_fn();
  printf("Child `net` Namespace:\n");
  system("ip link");
  printf("\n\n");

}

static int child_fn_nodename () {
  child_fn();

  printf("Child UTS namespace nodename: ");
  print_nodename();

  printf("Changing nodename inside child UTS namespace to GLaDOS\n");
  sethostname("GLaDOS", 6);

  printf("Check child UTS namespace nodename: ");
  print_nodename();

  return 0;
}


void run_clone_type(char* env_var_content) {

  printf("Env var CLONE_TYPE = %s\n", env_var_content);

  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "SIGCHLD") == 0)) {
    printf("\n\nRunning clone type SIGCHLD\n");
    pid_t child_pid = clone(child_fn, child_stack+1048576, SIGCHLD, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }

  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_CHILD_SETTID") == 0)) {
    printf("\n\nRunning clone type CLONE_CHILD_SETTID\n");
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_CHILD_SETTID, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }

  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_CHILD_CLEARTID") == 0)) {
    printf("\n\nRunning clone type CLONE_CHILD_CLEARTID\n");
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_CHILD_CLEARTID, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }

  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD") == 0)) {
    printf("\n\nRunning clone type CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD\n");
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }



  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_NEWPID") == 0)) {
    printf("\n\nRunning clone type CLONE_NEWPID\n");
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWPID, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }


  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_NEWPID | SIGCHLD") == 0)) {
    printf("\n\nRunning clone type CLONE_NEWPID | SIGCHLD\n");
    pid_t child_pid = clone(child_fn, child_stack+1048576, CLONE_NEWPID | SIGCHLD, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }

  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_NEWPID | CLONE_NEWNET | SIGCHLD") == 0)) {
    printf("\n\nRunning clone type CLONE_NEWPID | CLONE_NEWNET | SIGCHLD\n");
    printf("Original `net` Namespace:\n");
    system("ip link");
    printf("\n\n");

    pid_t child_pid = clone(child_fn_namespace, child_stack+1048576, CLONE_NEWPID | CLONE_NEWNET | SIGCHLD, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
  }

  if((strcmp(env_var_content, "ALL") == 0 || strcmp(env_var_content, "CLONE_NEWUTS | SIGCHLD") == 0)) {
    printf("\n\nRunning clone type CLONE_NEWUTS | SIGCHLD\n");
    printf("Original UTS namespace nodename before any cloning: ");
    print_nodename();
    pid_t child_pid = clone(child_fn_nodename, child_stack+1048576, CLONE_NEWUTS | SIGCHLD, NULL);
    printf("clone() = %ld\n", (long)child_pid);
    waitpid(child_pid, NULL, 0);
    printf("Original UTS namespace nodename after all cloning: ");
    print_nodename();
  }

}

/*
gcc -o ns-newpid ns-newpid.c
export CLONE_TYPE=SIGCHLD
./ns-newpid
export CLONE_TYPE=CLONE_CHILD_SETTID
./ns-newpid
export CLONE_TYPE=CLONE_CHILD_CLEARTID
./ns-newpid
export CLONE_TYPE="CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD"
./ns-newpid
export CLONE_TYPE=SIGCHLD
./ns-newpid
# Below we need root access
export CLONE_TYPE=CLONE_NEWPID
./ns-newpid
export CLONE_TYPE="CLONE_NEWPID | SIGCHLD"
./ns-newpid
export CLONE_TYPE="CLONE_NEWPID | CLONE_NEWNET | SIGCHLD"
./ns-newpid
export CLONE_TYPE="CLONE_NEWUTS | SIGCHLD"
./ns-newpid
*/
int main() {
  run_clone_type(getenv("CLONE_TYPE"));
  return 0;
}
