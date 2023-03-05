FROM ubuntu:22.04

RUN echo "hello"
RUN apt-get update
RUN apt-get install -y build-essential git cmake libcmocka0 libcmocka-dev \
    libgl-dev libglew-dev libglfw3-dev libstb-dev libglm-dev \
    bash-completion

RUN mkdir -p /root/odfe/build
COPY .git /root/odfe/.git

WORKDIR /root/odfe

RUN git reset --hard HEAD

WORKDIR /root/odfe/build
RUN cmake ..
RUN make check

