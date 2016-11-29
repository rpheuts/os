FROM gcc:4.9

RUN apt-get update
RUN apt-get install -y nasm libmpfr-dev

COPY install.sh /usr/local/bin/install.sh
COPY build.sh /usr/local/bin/build.sh

RUN chmod +x /usr/local/bin/install.sh
RUN chmod +x /usr/local/bin/build.sh

RUN bash install.sh

ENTRYPOINT ["/usr/local/bin/build.sh"]