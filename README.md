# ICFPC2017 Team Adlersprung

## Members
- mkut
- Osamu Koga
- Shunsuke Ohashi
- Yosuke Yano
- yuf
- Yuki Kawata

## Our strategy
Main AI is src/ai/kawatea/cherry_pick.cpp. It splits the game into three phase:

1. First phase: picks rivers that are located near from mines. It is not only to maximize the chance to get as many
   mines as possible, but also to block other players from taking high-efficiency rivers.
2. Middle phase: extends paths so that mines are connected together, as well as makes those paths longer.
3. Final phase: once the board becomes filled up and no many choices remained, greedily chooses a river that gains the
   most score at that point.

## Remarks
"Isn't it Ticket to Ride, huh?" -- mkut
