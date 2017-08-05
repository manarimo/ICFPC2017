#!/usr/bin/env ruby

require 'json'
require 'open3'
require 'pp'

class River < Struct.new(:source, :target, :owner)
  def self.from_json(json)
    self.new(json['source'], json['target'], json['owner'] || -1)
  end

  def hash
    [source, target].hash
  end

  def eql?(o)
    [source, target].eql?([o.source, o.target])
  end

  def to_hash
    {
        source: source,
        target: target,
        owner: owner
    }
  end
end

class Map < Struct.new(:sites, :rivers, :mines)
  attr_reader :site_to_id, :river_to_id, :id_to_river
  def initialize(sites, rivers, mines)
    super(sites, rivers, mines)

    @site_to_id = {}
    sites.each_with_index do |s, i|
      @site_to_id[s]= i
    end
    @river_to_id = {}
    @id_to_river = {}
    rivers.each_with_index do |river, i|
      @river_to_id[river] = i
      @id_to_river[i] = river
    end
  end

  def self.from_json(json)
    self.new(
        json['sites'].map{|s| s['id']},
        json['rivers'].map{|r| River.from_json(r)},
        json['mines'])
  end

  def edge(source, target)
    river_to_id[River.new(source, target)]
  end

  def edge_of(edge_id)
    id_to_river[edge_id]
  end

  def set_owner(punter_id, edge_id)
    id_to_river[edge_id].owner = punter_id
  end

  def to_kyopro
    <<"END"
#{site_to_id.size}
#{mines.size}
#{mines.map{|m| site_to_id[m]}.join(' ')}
#{rivers.size}
#{rivers.map{|r| "#{site_to_id[r.source]} #{site_to_id[r.target]} #{r.owner}"}.join("\n")}
END
  end

  def to_hash
    {
        sites: sites.map{|s| {id: s}},
        rivers: rivers.map{|r| r.to_hash},
        mines: mines
    }
  end
end

class State < Struct.new(:my_id, :punters, :map, :settings, :app_state)
  def self.from_json(json)
    settings = {}
    if json.key?('settings')
      settings = json['settings']
    end
    self.new(json['punter'], json['punters'], Map.from_json(json['map']), settings, json['app_state'])
  end

  def to_kyopro
    settings_list = []
    settings.each do |key, value|
      if value
        settings_list.push(key)
      end
    end
    <<"END"
#{punters}
#{my_id}
#{map.to_kyopro}
#{settings_list.size}
#{settings_list.join(' ')}
#{app_state || ''}
END
  end

  def to_hash
    {
        punter: my_id,
        punters: punters,
        map: map.to_hash,
        settings: settings,
        app_state: app_state
    }
  end
end

class Move < Struct.new(:action, :punter, :edge)
  def self.from_json(json, map)
    if json.key?('claim')
      from = json['claim']['source'].to_i
      to = json['claim']['target'].to_i
      edge_id = map.edge(from, to)
      self.new(:claim, json['claim']['punter'], edge_id)
    else
      self.new(:pass, json['punter'], nil)
    end
  end

  def to_hash(map)
    {
        claim: {
            punter: punter,
            source: map.edge_of(edge).source,
            target: map.edge_of(edge).target
        }
    }
  end
end

class GamePlay < Struct.new(:moves, :state)
  def self.from_json(json)
    state = State.from_json(json['state'])
    moves = json['move']['moves'].map{|m| Move.from_json(m, state.map)}
    moves.select{|m| m.action == :claim}.each do |move|
      state.map.set_owner(move.punter, move.edge)
    end
    self.new(moves, state)
  end

  def to_kyopro
    state.to_kyopro
  end
end

class Reader
  def initialize(stream)
    @stream = stream
    @buf = nil
  end

  def read_json
    if @buf.nil?
      @buf = @stream.read
    end
    if m = @buf.match(/^(\d+):/)
      len = m[1].to_i
      json_str = m.post_match[0, len]
      @buf = m.post_match[len...m.post_match.size]
      JSON.parse(json_str)
    else
      raise "Not enough input. Remaining: #{@buf}"
    end
  end
end

def read_json(stdin)
  buf = stdin.read
  m = buf.match(/^(\d+):/)
  size = m[1].to_i
  total_size = m[0].size + size
  while buf.size < total_size
    buf << stdin.read
  end
  buf.gsub!(/^\d+:/, '')
  JSON.parse(buf)
end

def print_json(stdout, obj)
  txt = obj.to_json
  stdout.puts "#{txt.size}:#{txt}"
  stdout.flush
end

reader = Reader.new(STDIN)
Open3.popen3(ARGV[0]) do |stdin, stdout, stderr|
  stdin.puts 'HANDSHAKE'
  stdin.close
  my_name = stdout.read
  payload = {
    me: my_name
  }
  print_json(STDOUT, payload)
  reader.read_json
end
Open3.popen3(ARGV[0]) do |stdin, stdout, stderr|
  json = reader.read_json
  if json.key?('punter')
    obj = State.from_json(json)
    stdin.puts 'INIT'
    stdin.puts obj.to_kyopro
    stdin.close
    num_futures = stdout.gets.to_i
    futures = []
    for i in 0...num_futures
      source, target = stdout.gets.split().map(&:to_i)
      futures.push({source: source, target: target})
    end
    buf = stdout.read
    obj.app_state = buf
    payload = {
      ready: obj.my_id,
      futures: futures,
      state: obj.to_hash
    }
    print_json(STDOUT, payload)
  elsif json.key?('move')
    obj = GamePlay.from_json(json)
    stdin.puts 'MOVE'
    stdin.puts obj.to_kyopro
    stdin.close
    edge_id = stdout.gets.to_i
    obj.state.app_state = stdout.read
    obj.state.map.set_owner(obj.state.my_id, edge_id)
    payload = {
        state: obj.state.to_hash
    }.merge(Move.new(:claim, obj.state.my_id, edge_id).to_hash(obj.state.map))
    print_json(STDOUT, payload)
  elsif json.key?('stop')
  end
  STDERR.puts stderr.read
end
