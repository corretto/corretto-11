FROM amazonlinux:2

RUN yum install -y yum-utils rpm-build

RUN cd /opt \
    && curl --remote-name https://download.java.net/java/GA/jdk11/9/GPL/openjdk-11.0.2_linux-x64_bin.tar.gz \
    && tar xf openjdk-11.0.2_linux-x64_bin.tar.gz \
    && rm openjdk-11.0.2_linux-x64_bin.tar.gz

WORKDIR rpmbuild

COPY SOURCES SOURCES
COPY java-11-amazon-corretto.spec .

RUN yum-builddep -y java-11-amazon-corretto.spec

ENV JAVA_HOME /opt/jdk-11.0.2

RUN cd /rpmbuild && rpmbuild --define "_topdir /rpmbuild" -ba java-11-amazon-corretto.spec
