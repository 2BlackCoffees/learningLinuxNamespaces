FROM ubuntu:20.04
USER root
RUN apt update -y
RUN apt install build-essential -y
RUN apt-get install strace -y
RUN mkdir cloning && chown 100:100 cloning
USER 100:100
WORKDIR cloning
ADD ns-newpid.c .
RUN gcc -o ns-newpid ns-newpid.c
ENTRYPOINT [ "sleep" , "20000"]