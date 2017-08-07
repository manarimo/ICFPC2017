# ICFPC2017 Team Adlersprung

Adlersprung (Eagle Jump in English) is a Germany game company, which is remarked by its success in Fairies Story.
This company is famous not only by its amazing quality of products, but also by the fact that employees are not limited to humans.
There are many ["Friends"](http://kemono-friends.wikia.com/wiki/Friends) working in this company as well.
Those "Friends" helped us a lot during this year's ICFP contest by
[automatically scheduling random matches for evaluation](https://github.com/osak/ICFPC2017/blob/readme/src/python/fennec/__main__.py),
[visualizing matches](https://github.com/osak/ICFPC2017/tree/readme/alpaca), [analyzing match results](https://github.com/osak/ICFPC2017/tree/readme/konohazuku)
or [communicating with online server](https://github.com/osak/ICFPC2017/blob/readme/src/python/serval_cat/__main__.py).

## Members
- mkut
- Osamu Koga
- Shunsuke Ohashi
- Yosuke Yano
- yuf
- Yuki Kawata

## Our strategy
We developed several punters and selected the strongest punter for the submission.
* kawatea-careful-combined (by Yuki): This punter is good at connecting mines each other. It recognizes the importance of edges
 (Larger connection cost if the important edge is not available. This is one of generalization of bridge in graph theory).
 This punter also disturbs others if possible.

We also developed punters that are not used in final submit. Please let us mention them. They are also our cute children!
* mu (by yuf): Monte Carlo Tree Search based method. A second is too short to collect enough playouts.
* poseidon (by mkut): This punter tries to analyze entire board that is not possible in a second for large maps.
* time-vault (by osak): Simple greedy splurge punter. Attempts to connect two mines whose distance is as far as possible. 
This is not used just because it is weak.
* kawatea-random (by Yosuke): This is a dummy opponent for evaluation purpose. We evaluated our punter using the score against this punter.
* jaguar (by Shunsuke): Machine learning based evaluation function. Feature extraction is too slow to finish within a second.
* artemis (by mkut): This expected score based punter is as strong as kawatea-careful-combined. 
Our first submission plan was to combine this and kawatea-careful-combined, but we decided not to use this.
We found a critical bug 30 minutes before the end of the contest :(

## Friends and other tools
* Alpaca (by Osamu): Match visualizer
* Araisan (Raccoon, by Shunsuke): slack wrapper
* Beaver (by Shunsuke): Match log postprocessor
* Fennec (by Shunsuke): Random match scheduler
* Kaban (by Osamu): Database Persister
* Konohazuku (scops owl, by Shunsuke): Punter rating calculator
* Serval Cat (by Yosuke): Home-made Online wrapper
* Tsuchinoko (an imaginary in Japan, by Yosuke): Punter evaluator using Random punter
* Washimimizuku (eagle owl, by Shunsuke): Evaluation function trainer for jaguar


* Zeus (by mkut): Offline match server.


* Kadingel: Jenkins server
* Japari Library: MySQL server


## Remarks
"Isn't it Ticket to Ride, huh?" -- mkut


