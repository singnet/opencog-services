(use-modules (opencog miner))

(define (execute args)
    (define mf (if (> (length args) 0) (car args) 0.1))
    (define ms (if (> (length args) 1) (cadr args) 5))
    (define mi (if (> (length args) 2) (caddr args) 100))
    (define mc (if (> (length args) 3) (cadddr args) 3))
    (define mv (if (> (length args) 4) (cadddr args) 2))

    ;(list ms mi ie mc)
    (cog-mine (cog-atomspace)
                #:minfreq mf
                #:minsup ms
                #:maximum-iterations mi
                #:conjunction-expansion #t
                #:max-conjuncts mc
                #:max-variables mv
                #:surprisingness 'nisurp)
)