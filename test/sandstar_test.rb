require_relative '../bin/sandstone'
require 'json'
require 'pp'

json = JSON.parse(File.read('gameplay.json'))
gp = GamePlay.from_json(json)
pp gp

json = JSON.parse(File.read('setup.json'))
s = State.from_json(json)
pp s
puts s.to_kyopro
