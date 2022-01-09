FROM ubuntu:16.04
ARG host_usrid
ARG host_usrname
ARG host_grpid
ARG host_grpname
ENV TZ=Europe/Brussels
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update --fix-missing
RUN apt-get install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 tmux libncurses-dev locales
RUN groupadd -g $host_grpid $host_grpname && useradd -u $host_usrid -g $host_grpname -d /home/$host_usrname $host_usrname && mkdir /home/$host_usrname && chown -R $host_usrname:$host_grpname /home/$host_usrname
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
USER $host_usrname
SHELL ["/bin/bash", "-c"]
WORKDIR /home/$host_usrname/poky
ENTRYPOINT TEMPLATECONF=meta-builderbot/conf source oe-init-build-env; /bin/bash
