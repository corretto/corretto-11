There are two subprojects related to AL2.

The **:spec** subproject generates a source tarball and RPM spec
suitable for building on a koji fleet. To execute this, run:
`./gradlew :installers:linux:al2:spec:build`. The spec file
and tarball are located in `/installers/linux/al2/spec/corretto-build/distributions`.


```
➜ ./gradlew :installers:linux:al2:spec:build
➜ tree installers/linux/al2/spec/corretto-build/distributions
  installers/linux/al2/spec/corretto-build/distributions
  └── amazon-corretto-source-11.0.2.7.1.tar.gz


```

The **:docker** subproject uses the spec and source tarball to build
RPMs within a docker container. To execute this, run:
`./gradlew :installers:linux:al2:docker:build`. The generated RPMs are
 located in `/installers/linux/al2/docker/corretto-build/distributions`.


```
➜ ./gradlew :installers:linux:al2:docker:build
➜ tree installers/linux/al2/docker/corretto-build/distributions
   installers/linux/al2/docker/corretto-build/distributions
   ├── java-11-amazon-corretto-11.0.2+7-1.amzn2.x86_64.rpm
   └── java-11-amazon-corretto-headless-11.0.2+7-1.amzn2.x86_64.rpm
```
