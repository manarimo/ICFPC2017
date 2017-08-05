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
Main AI is src/ai/kawatea/cherry_pick.cpp. It splits the game into three phases:

1. First phase: picks rivers that are located near from mines. It is not only to maximize the chance to get as many
   mines as possible, but also to block other players from taking high-efficiency rivers.
2. Middle phase: extends paths so that mines are connected together, as well as makes those paths longer.
3. Final phase: once the board becomes filled up and no many choices remained, greedily chooses a river that gains the
   most score at that point.
   
## Remarks
"Isn't it Ticket to Ride, huh?" -- mkut


