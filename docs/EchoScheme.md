[issue-template]: ../../../issues/new?template=BUG_REPORT.md
[feature-template]: ../../../issues/new?template=FEATURE_REQUEST.md
[opencog-services-repo]: https://github.com/singnet/opencog-services
[general-guide]: ./opencog-services.md
[dap]: http://alpha.singularitynet.io/
[scheme]: https://wiki.opencog.org/wikihome/index.php/Scheme

![singnetlogo](assets/singnet-logo.jpg?raw=true 'SingularityNET')

# EchoScheme

This service "echoes" its arguments.

It is part of our [Opencog Services][opencog-services-repo].

### Welcome

The service expects 0 to n arguments (n is limited as described [here](general-guide)) and simply outputs a list of them in [Scheme][scheme] syntax.

### What’s the point?

The service is meant to illustrate the implementation of a Scheme Opencog service.

### How does it work?

You can use this service from [SingularityNET DApp][dap], clicking on `SNET/Opencog`.

You can also call the service from SingularityNET CLI (`snet`):

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call execute EchoScheme 1 2
```

It can also be executed as an asynchronous task:

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call asynchronousTask Echo 1 2
```

In this case the command output is an URL where you can find the results as soon as they are available.

### Issues and feature requests

Issues can be reported [here](issue-template). New features can be requested [here](feature-template).
