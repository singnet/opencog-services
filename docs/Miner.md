[opencog-services-repo]: https://github.com/singnet/opencog-services
[dap]: http://alpha.singularitynet.io/
[opencog-pattern-miner]: https://github.com/singnet/opencog/tree/master/opencog/learning/miner
[pattern-miner-repo]: https://github.com/singnet/opencog/tree/master/opencog/learning/miner
[opencog]: https://opencog.org/
[atomese]: https://wiki.opencog.org/w/Atomese
[soda-drinkers]: https://raw.githubusercontent.com/singnet/opencog/master/examples/learning/miner/ugly-male-soda-drinker/kb.scm
[opencog-tutorial]: https://github.com/singnet/wiki/tree/master/tutorials/howToWriteOpencogService
[singularitynet-home]: https://www.singularitynet.io
[contribution-guidelines]: https://github.com/singnet/wiki/blob/master/guidelines/CONTRIBUTING.md

[echo]: Echo.md

![singnetlogo](assets/singnet-logo.jpg?raw=true 'SingularityNET')

# Miner

This service loads a knowledge base into an instance of [Opencog][opencog] and run
[Opencog's Pattern Miner][opencog-pattern-miner]. The best ranked results are
returned as a list of atoms in [Atomese][Atomese].

It's part of our [Opencog Services](opencog-services-repo).

### Welcome

The service expects an URL to an Atomese knowledge base and 4 parameters which
are passed to pattern miner itself to control its behavior (see
[here][opencog-pattern-miner] for an explanation on how those parameters
control the pattern miner algorithm.

Parameters:

1. **minsup -** Integer - Minimum support - the mining algorithm discards patterns with frequency below that value.
2. **maxiter -** Integer - Maximum iterations - limits the number of iterations of the mining algorithm.
3. **incremental-expansion -** - Boolean (#t or #f) - allows increasing number of conjuncts.
4. **max-conjuncts -** - Integer - limits the number of conjunctions considered by the mining algorithm.

A simple example of knowledge base is presented [here][soda-drinkers].

### What’s the point?

The service is meant to illustrate the implementation of an actual Opencog
service. Unlike other example services (like [echo](Echo)), this service does
some relevant Opencog processing.

### How does it work?

You can use this service from [SingularityNET DApp][dap], clicking on `SNET/Opencog`.

You can also call the service from SingularityNET CLI (`snet`):

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call execute Miner KNOWLEDGE_BASE_URL MIN_SUP MAX_ITER INCR_EXP MAX_CONJ
```

The following command
line will call the Pattern Miner on the [Ugly male soda-drinkers][soda-drinkers] knowledge base:

```
$ snet client call execute Miner https://raw.githubusercontent.com/singnet/opencog/master/examples/learning/miner/ugly-male-soda-drinker/kb.scm 5 100 \#t 3
```

Pattern Miner can take several minutes (or even hours) to execute depending on
the size of the knowledge base and the parameters (max-conjuncts is a
specially critical parameter). See [Pattern Miner Documentation][opencog-pattern-miner]
to understand how it works and how each parameter can affect the expected results.

So you may want to execute Pattern Miner asynchronously:

```
$ snet set current_agent_at YOUR_AGENT_ADDRESS
set current_agent_at YOUR_AGENT_ADDRESS

$ snet client call asynchronousTask Miner KNOWLEDGE_BASE_URL MIN_SUP MAX_ITER INCR_EXP MAX_CONJ
```

In this case the command output is an URL where you can find the results as
soon as they are available. Results will remain available for a limited time
(see [general-guide](Opencog Services documentation) for details).

### What to expect from this service?

For the [Ugly male soda-drinkers][soda-drinkers] knowledge base, Pattern Miner (with aforementioned parameters) should output something like:

```
(SetLink
   (LambdaLink
      (VariableList
         (VariableNode "$PM-793e6063")
         (VariableNode "$PM-762382f3")
      )
      (AndLink
         (InheritanceLink
            (VariableNode "$PM-793e6063")
            (VariableNode "$PM-762382f3")
         )
         (InheritanceLink
            (VariableNode "$PM-793e6063")
            (ConceptNode "ugly")
         )
         (InheritanceLink
            (ConceptNode "Cason")
            (VariableNode "$PM-762382f3")
         )
      )
   )
   (LambdaLink
      (VariableNode "$PM-793e6063")
      (AndLink
         (InheritanceLink
            (VariableNode "$PM-793e6063")
            (ConceptNode "man")
         )
         (InheritanceLink
            (VariableNode "$PM-793e6063")
            (ConceptNode "soda drinker")
         )
      )
   )

  ...
```

### Contributing and Reporting Issues

Please read our [guidelines][contribution-guidelines] before
submitting issues or pull requests.

## Authors

* SNET service - **Andre Senna** - *Maintainer* - [SingularityNET][singularitynet-home]
* Opencog's Pattern Miner - see [here][pattern-miner-repo]
