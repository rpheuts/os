FROM gcc:4.9

RUN apt-get update
RUN apt-get install -y nasm libmpfr-dev

COPY install.sh /
RUN bash /install.sh