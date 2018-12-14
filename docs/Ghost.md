[opencog-services-repo]: https://github.com/singnet/opencog-services
[dap]: http://alpha.singularitynet.io/
[opencog-tutorial]: https://github.com/singnet/wiki/tree/master/tutorials/howToWriteOpencogService
[singularitynet-home]: https://www.singularitynet.io
[contribution-guidelines]: https://github.com/singnet/wiki/blob/master/guidelines/CONTRIBUTING.md

![singnetlogo](assets/singnet-logo.jpg?raw=true 'SingularityNET')

# Echo

This service allows your to have a conversation with an OpenCog's Ghost based chatbot.

It's part of our [Opencog Services][opencog-services-repo].

### Welcome

The service expects 0 to n arguments (n is limited as described [here](opencog-tutorial)) and simply outputs then separated by single spaces.

### What’s the point?

The service is meant to illustrate the implementation of a C++ GHOST chatbot, an Opencog service, that can handle multiple exclusive OpenCog sessions.
It shows how opencog's AtomSpaces sets can possibly be managed independently by several MindAgents and used together to form a higher form of AI.
It's also useful to test availability of the server.

### How does it work?

You can use this service from [SingularityNET DApp][dap], clicking on `SNET/Opencog`.

You can also call the service from SingularityNET CLI (`snet`):

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call execute Ghost
```

For this case you should receive the following response: "I am Betty"
This is the current name given to this GHOST based chatbot service.

The service deploys GHOST sessions based on the provided OpenCog's service api, where each one holds a private and exclusive AtomSpace, Scheme Evaluator, and CogServer to run MindAgents locally. It allow multiple agents with different personas to be handled by the same program. Moreover, they can be used as sessions for different users.
Multiple GHOST sessions can be created and they are completely independent, since they run in separate processes. 

In order to load rule files and start one session for this GHOST service, you can use the following command:

```
$ snet client call execute Ghost start_session https://raw.githubusercontent.com/Ophien/rulefiles/master/test.scm
```

For this command, Ghost will create a new session and load the rule file located at "https://raw.githubusercontent.com/Ophien/rulefiles/master/test.scm" to its AtomSpace.

In order to have a conversation with GHOST in a specific session, use the following command:

```
$ snet client call execute Ghost utterance <session_id> "hello!"
```

where <session_id> should be replaced by an id of a previsouly starded session.
For this command, GHOST will receive the utterance "hello!", analyze it through the Scheme Evaluator and AtomSpace for the specified <session_id>, and will provide a response.

Sessions can be closed by performing the following command:

```
$ snet client call execute Ghost end_session <session_id>
```

where <session_id> should be replaced by an id of a previsouly starded session.


### Contributing and Reporting Issues

Please read our [guidelines][contribution-guidelines] before
submitting issues or pull requests.

## Authors

* **Alysson Ribeiro da Silva** - *Maintainer* - [SingularityNET][singularitynet-home]
