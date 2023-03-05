FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install -y build-essential git cmake libgl-dev libgl1 libglm-dev libglfw3-dev libglfw3 libglew-dev libglew2.2  libcmocka-dev libcmocka0 libstb-dev

RUN mkdir -p /root/odfe/build
COPY .git /root/odfe/.git

WORKDIR /root/odfe

RUN git reset --hard HEAD

WORKDIR /root/odfe/build
RUN cmake ..
RUN make check

