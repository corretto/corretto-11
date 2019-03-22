FROM amazoncorretto:11
COPY rpmbuild /rpmbuild
RUN yum install -y yum-utils rpm-build \
    && yum-builddep -y rpmbuild/java-11-amazon-corretto.spec
ENV JAVA_HOME /usr/lib/jvm/java-11-amazon-corretto
RUN cd /rpmbuild && rpmbuild --define "_topdir /rpmbuild" -ba java-11-amazon-corretto.spec

FROM amazonlinux:2
COPY --from=0 /rpmbuild/RPMS/x86_64 .
RUN yum -y localinstall *.rpm \
    && yum install -y man

COPY post-install-test.sh .
RUN /post-install-test.sh
