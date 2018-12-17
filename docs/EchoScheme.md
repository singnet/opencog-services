[opencog-services-repo]: https://github.com/singnet/opencog-services
[dap]: http://alpha.singularitynet.io/
[scheme]: https://wiki.opencog.org/wikihome/index.php/Scheme
[opencog-tutorial]: https://github.com/singnet/wiki/tree/master/tutorials/howToWriteOpencogService
[singularitynet-home]: https://www.singularitynet.io
[contribution-guidelines]: https://github.com/singnet/wiki/blob/master/guidelines/CONTRIBUTING.md

![singnetlogo](assets/singnet-logo.jpg?raw=true 'SingularityNET')

# EchoScheme

This service "echoes" its arguments.

It's part of our [Opencog Services][opencog-services-repo].

### Welcome

The service expects 0 to n arguments (n is limited as described [here](opencog-tutorial)) and simply outputs a list of them in [Scheme][scheme] syntax.

### What’s the point?

The service is meant to illustrate the implementation of a Scheme Opencog service.

### How does it work?

You can use this service from [SingularityNET DApp][dap], clicking on `SNET/Opencog`.

You can also call the service from SingularityNET CLI (`snet`). See
[here][snet-doc] for detailed information on how SingularityNET CLI works or
use the helper scripts from opencog-services repository as described below.

Clone the repository to use the scripts.

```
git clone git@github.com:singnet/opencog-services.git
```

First, make sure your session have an identity with your private key.

```
$ snet identity create YOUR_ID key
```

`YOUR_ID` is a local id used only in the current session so it's not really
important. Any string should work. You will be requested to enter your private
key.

Now you need to add funds to a MPE channel.

```
$ ./scripts/deposit_mpe.sh
```

This script will print the balances and a list of all chanels of your identity.
Look for the `#channelId` of the last listed channel. This is the channel which
will be used to make the client calls below.

```
$ ./scripts/client_request.sh EchoScheme foo bar
```

### Contributing and Reporting Issues

Please read our [guidelines][contribution-guidelines] before
submitting issues or pull requests.


## Authors

* **Andre Senna** - *Maintainer* - [SingularityNET][singularitynet-home]
