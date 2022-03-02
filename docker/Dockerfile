FROM ubuntu:16.04
ARG host_user_id
ARG host_group_id
ENV TZ=Europe/Brussels
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update --fix-missing
RUN apt-get install -y gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 tmux libncurses-dev locales
RUN groupadd -g $host_group_id developer && useradd -u $host_user_id -g developer -d /home/developer developer
RUN mkdir /home/developer && chown -R developer:developer /home/developer
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
USER developer
SHELL ["/bin/bash", "-c"]
WORKDIR /home/developer/poky
ENTRYPOINT TEMPLATECONF=meta-builderbot/conf source oe-init-build-env; /bin/bash
