![singnetlogo](assets/singnet-logo.jpg 'SingularityNET')

[singularitynet-home]: https://www.singularitynet.io
[contribution-guidelines]: https://github.com/singnet/wiki/blob/master/guidelines/CONTRIBUTING.md
[cpp-tutorial]: https://github.com/singnet/wiki/tree/master/tutorials/howToWriteCPPService
[opencog-tutorial]: https://github.com/singnet/wiki/tree/master/tutorials/howToWriteOpencogService
[opencog-pattern-miner]: https://github.com/singnet/opencog/tree/master/opencog/learning/miner
[users-guide]: https://singnet.github.io/opencog-services/


[![CircleCI](https://circleci.com/gh/singnet/opencog-services.svg?style=svg)](https://circleci.com/gh/singnet/opencog-services)

# Opencog services to SingularityNET

This repository contains a number of SingularityNET services which uses Opencog resources.

Actually, we have only one C++ gRPC service which is used to call any of the
implemented services passing a command name and a list of arguments.

C++ and Scheme services are implemented in `src/cpp-services` and
`src/scm-services` respectively.

- [Echo](docs/Echo.md) - a simple 'echo' which just outputs its input, used to test the architecture and to serve as example.
- [EchoScheme](docs/EchoScheme.md) - the same, but written in `Scheme`.
- [Miner](docs/Miner.md) - load a knowledge base and run [Opencog's PatternMiner][opencog-pattern-miner]
- [Ghost](docs/Ghost.md) - handle several OpenCog servers locally, to operate MindAgents, and allow to handle GHOST agents in exclusive sessions.

[HTML User's Guide Hub][users-guide]

## Getting Started

In addition to the services themselves, this repository contains all the basic
code and scripts required to build and deploy the Opencog services.

### Building and testing services locally (without blockchain)

The below setup requires docker and will build docker images with all the required packages.
Having a docker container is useful because the Opencog Services uses C++ gRPC bindings
which requires a lot of pre-requisite packages to be installed.

```
git clone git@github.com:singnet/opencog-services.git
cd opencog-services
# make sure you have the latest base image
docker pull opencog/opencog-dev:cli
docker image build -t opencog_services_basic -f basic-dockerfile .
docker image build -t opencog_services -f Dockerfile .
docker run --name OPENCOG_SERVICE_DEV -ti opencog_services /bin/bash
```

Find `server` and `client` executables in `bin/`

You can run tests locally. Start a `server`

```
$ ./bin/server &
```

and make any number of requests using `client`

```
$ ./bin/client sync Echo foo bar
foo bar
```

The command line above is requesting the execution of the service `Echo`,
passing two arguments `foo` and `bar`. The service just echoes the passed input.

The keyword `sync` before the service name states that the call should be
synchronous. So the `client` command will wait for the service to finish before
returning. You can make an asynchronous call using

```
$ ./bin/client async Echo foo bar
Task "Echo" started. See results in: http://54.203.198.53:7000/ServiceAsynchronousOutput/opencog/Echo-53171
```

In this case the service return an URL which can be acessed asynchronously to
get the results (when they are ready). This is specially useful when running
services that take too long to execute.

**Important note:** `async` calls are setup to work only in the deployment
container. To test asynchronous commands locally you need to change
src/server.cc:

```
#define ASYNCHRONOUS_API_OUTPUT_URL 
#define ASYNCHRONOUS_API_OUTPUT_DIR 

```

In the deployment environment, results remain available for 24 hours after being delivered. Then they are
deleted.

### Writing a new Opencog service

[This tutorial][opencog-tutorial] explains how to write a new Opencog-based service.

### Contributing and Reporting Issues

Please read our [guidelines][contribution-guidelines] before
submitting issues or pull requests. 


## Authors

* **Andre Senna** - *Maintainer* - [SingularityNET][singularitynet-home]

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
