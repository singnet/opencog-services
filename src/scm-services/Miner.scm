(use-modules (opencog miner))

(define (execute args)
    (define ms (if (> (length args) 0) (car args) 5))
    (define mi (if (> (length args) 1) (cadr args) 100))
    (define ie (if (> (length args) 2) (caddr args) #t))
    (define mc (if (> (length args) 3) (cadddr args) 3))
    ;(list ms mi ie mc)
    (cog-mine (cog-atomspace) #:minsup ms #:maxiter mi #:incremental-expansion ie #:max-conjuncts mc)
)