(use-modules (opencog miner))

(define (execute args)
    (define ms (if (> (length args) 0) (car args) 5))
    (define mi (if (> (length args) 1) (cadr args) 100))
    (define ce (if (> (length args) 2) (caddr args) #t))
    (define mc (if (> (length args) 3) (cadddr args) 3))
    (define mv (if (> (length args) 4) (cadddr args) 2))
    (define sp (if (> (length args) 5) (cadddr args) 'nisurp))
    
    ;(list ms mi ie mc)
    (cog-mine (cog-atomspace) #:minsup ms #:maximum-iterations mi #:conjunction-expansion ce #:max-conjuncts mc #:max-variables #:surprisingness sp)
)
