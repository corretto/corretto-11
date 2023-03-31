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
