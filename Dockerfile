FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y build-essential curl zip unzip tar git flex bison

WORKDIR /mcpp
COPY / /mcpp

RUN make system