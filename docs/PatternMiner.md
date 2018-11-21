[issue-template]: ../../../issues/new?template=BUG_REPORT.md
[feature-template]: ../../../issues/new?template=FEATURE_REQUEST.md
[opencog-services-repo]: https://github.com/singnet/opencog-services
[general-guide]: ./opencog-services.md
[dap]: http://alpha.singularitynet.io/
[opencog-pattern-miner]: https://wiki.opencog.org/w/Pattern_miner
[opencog]: https://opencog.org/
[atomese]: https://wiki.opencog.org/w/Atomese
[soda-drinkers]: https://github.com/singnet/opencog/blob/master/opencog/learning/PatternMiner/ugly_male_soda-drinker_corpus.scm
[opencog-parameters]: https://github.com/singnet/opencog/blob/master/lib/opencog_patternminer.conf
[echo]: Echo.md


![singnetlogo](../assets/singnet-logo.jpg?raw=true 'SingularityNET')

# PatternMiner

This service loads a knowledge base into an instance of [Opencog](opencog) and run
[Opencog's PatternMiner][opencog-pattern-miner]. The best ranked results are
returned as a list of atoms in [Atomese](Atomese).

It's part of our [Opencog Services](opencog-services-repo).

### Welcome

The service expects an URL to an Atomese knowledge base. Optionally a second
parameter may be provided passing a JSON hash with values to Opencog's
configuration parameters.

A simple example of knowledge base is presented [here](soda-drinkers).

A list of all relevant parameters to Opencog and the Pattern Miner is presented [here](opencog-parameters)

### What’s the point?

The service is meant to illustrate the implementation of an actual Opencog
service. Unlike other example services (like [echo](Echo)), this service does
some relevant Opencog processing.

### How does it work?

You can use this service from [SingularityNET DApp](dap), clicking on `SNET/Opencog`.

You can also call the service from SingularityNET CLI (`snet`):

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call execute PatternMiner KNOWLEDGE_BASE_URL
```

A JSON hash can optionally be passed to set parameters. The following command
line will call the Pattern Miner on the [Ugly male soda-drinkers](soda-drinkers) knowledge base setting `Max_thread_num = 8` and `Pattern_Max_Gram = 3`.

```
$ snet client call execute PatternMiner https://github.com/singnet/opencog/blob/master/opencog/learning/PatternMiner/ugly_male_soda-drinker_corpus.scm '{"Max_thread_num": "8", "Pattern_Max_Gram": "3"}'
```

Pattern Miner can take several minutes (or even hours) to execute depending on
the size of the knowledge base and the parameters (`Pattern_Max_Gram` is a
specially critical parameter). See [Pattern Miner Documentation][opencog-pattern-miner]
to understand how it works and how each parameter can affect the expected results.

So you may want to execute Pattern Miner asynchronously:

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call asynchronousTask PatternMiner KNOWLEDGE_BASE_URL
```

In this case the command output is an URL where you can find the results as
soon as they are available. Results will remain available for a limited time
(see [general-guide](Opencog Services documentation) for details).

### What to expect from this service?

For the [Ugly male soda-drinkers](soda-drinkers) knowledge base, Pattern Miner (with defauklt parameters) should outpu:

```
(InheritanceLink )
  (PatternVariableNode $var_1)
  (ConceptNode man)

(InheritanceLink )
  (PatternVariableNode $var_1)
  (ConceptNode soda drinker)

(InheritanceLink )
  (PatternVariableNode $var_1)
  (ConceptNode ugly)
```

### Issues and feature requests

Issues can be reported [here](issue-template). New features can be requested [here](feature-template).
