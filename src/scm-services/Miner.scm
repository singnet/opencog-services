(use-modules (opencog miner))
(use-modules (opencog bioscience))

(define (execute args)
    ; check parameters and add some default values in case them are not specified
    (define mf (if (> (length args) 0) (car args) 0.1))
    (define ms (if (> (length args) 1) (cadr args) 5))
    (define mi (if (> (length args) 2) (caddr args) 100))
    (define mc (if (> (length args) 3) (cadddr args) 3))
    (define mv (if (> (length args) 4) (cadddr args) 2))

    ; default values
    (define d-ms 5)
    (define d-mi 100)
    (define d-mc 3)
    (define d-mv 2)

    ; decide with it will use minfreq or minsup
    (define use-mf #t)

    ; check each parameter to guarantee the system will receive at least default values
    (if (<= mf 0)(set! use-mf #f))
    (if (<= ms 0)(set! ms d-ms  ))
    (if (<= mi 0)(set! mi d-mi  ))
    (if (<= mc 0)(set! mc d-mc  ))
    (if (<= mv 0)(set! mv d-mv  ))

    (if (eq? use-mf #t)
        ; call for the pattern miner with minfreq
        (cog-mine (cog-atomspace)
                    #:minfreq mf
                    #:maximum-iterations mi
                    #:conjunction-expansion #t
                    #:max-conjuncts mc
                    #:max-variables mv
                    #:surprisingness 'nisurp)

        ; call for the pattern miner with minsup
        (cog-mine (cog-atomspace)
                    #:minsup ms
                    #:maximum-iterations mi
                    #:conjunction-expansion #t
                    #:max-conjuncts mc
                    #:max-variables mv
                    #:surprisingness 'nisurp)
    )
)