#!/usr/bin/env ruby

require 'json'
require 'open3'
require 'stringio'
require 'pp'

class River < Struct.new(:source, :target, :owner1, :owner2)
  def self.from_json(json)
    self.new(json['source'], json['target'], json['owner1'] || -1, json['owner2'] || -1)
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
        owner1: owner1,
        owner2: owner2
    }
  end
end

class Map < Struct.new(:sites, :rivers, :mines)
  attr_reader :site_to_id, :id_to_site, :river_to_id, :id_to_river
  def initialize(sites, rivers, mines)
    super(sites, rivers, mines)

    @site_to_id = {}
    @id_to_site = {}
    sites.each_with_index do |s, i|
      @site_to_id[s]= i
      @id_to_site[i] = s
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
    river_to_id[River.new(source, target)] || river_to_id[River.new(target, source)]
  end

  def edge_of(edge_id)
    id_to_river[edge_id]
  end

  def site_of(site_id)
    id_to_site[site_id]
  end

  def set_owner(punter_id, edge_id)
    river = id_to_river[edge_id]
    if river.owner1 == -1
      river.owner1 = punter_id
    else
      river.owner2 = punter_id
    end
  end

  def to_kyopro
    <<"END"
#{site_to_id.size}
#{mines.size}
#{mines.map{|m| site_to_id[m]}.join(' ')}
#{rivers.size}
#{rivers.map{|r| "#{site_to_id[r.source]} #{site_to_id[r.target]} #{r.owner1} #{r.owner2}"}.join("\n")}
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

class State < Struct.new(:my_id, :punters, :map, :settings, :app_state, :app_name)
  def self.from_json(json)
    settings = {}
    if json.key?('settings')
      settings = json['settings']
    end
    self.new(json['punter'], json['punters'], Map.from_json(json['map']), settings, json['app_state'], json['app_name'])
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
        app_state: app_state,
        app_name: app_name
    }
  end
end

class Move < Struct.new(:action, :punter, :edge, :route)
  def self.from_json(json, map)
    if json['claim']
      from = json['claim']['source'].to_i
      to = json['claim']['target'].to_i
      edge_id = map.edge(from, to)
      self.claim(json['claim']['punter'], edge_id)
    elsif json['splurge']
      self.splurge(json['splurge']['punter'], json['splurge']['route'])
    elsif json['option']
      from = json['option']['source'].to_i
      to = json['option']['target'].to_i
      edge_id = map.edge(from, to)
      self.option(json['option']['punter'], edge_id)
    else
      self.pass(json['punter'])
    end
  end

  def self.claim(punter, edge)
    self.new(:claim, punter, edge, nil)
  end

  def self.pass(punter)
    self.new(:pass, punter, nil, nil)
  end

  def self.splurge(punter, sites)
    self.new(:splurge, punter, nil, sites)
  end

  def self.option(punter, edge)
    self.new(:option, punter, edge, nil)
  end

  def to_hash(map)
    STDERR.puts "action is #{action}"
    case action
      when :claim
        {
            claim: {
                punter: punter,
                source: map.edge_of(edge).source,
                target: map.edge_of(edge).target
            }
        }
      when :pass
        {
            pass: {
                punter: punter
            }
        }
      when :splurge
        {
            splurge: {
                punter: punter,
                route: route
            }
        }
      when :option
        {
            option: {
                punter: punter,
                source: map.edge_of(edge).source,
                target: map.edge_of(edge).target
            }
        }
      else
        raise "Unsupproted action #{action}"
      end
  end
end

class GamePlay < Struct.new(:moves, :state)
  def self.from_json(json)
    state = State.from_json(json['state'])
    moves = json['move']['moves'].map{|m| Move.from_json(m, state.map)}
    # STDERR.puts json
    moves.select{|m| m.action == :claim || m.action == :option}.each do |move|
      state.map.set_owner(move.punter, move.edge)
    end
    moves.select{|m| m.action == :splurge}.each do |splurge|
      splurge.route[0..-2].zip(splurge.route[1..-1]).each do |a, b|
        edge_id = state.map.edge(a, b)
        state.map.set_owner(splurge.punter, edge_id)
      end
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

def run(cmd, input)
  out, err = Open3.capture3(cmd, stdin_data: input)
  yield out, err
end

def determine_app(state)
  "#{__dir__}/../build/artemis"
end

reader = Reader.new(STDIN)

run(ARGV[0], "HANDSHAKE\n") do |out, err|
  my_name = out.chomp
  payload = {
    me: my_name
  }
  print_json(STDOUT, payload)
  reader.read_json
  # STDERR.puts err
end


json = reader.read_json
if json.key?('punter')
  obj = State.from_json(json)
  if ARGV[1] == 'prod'
    app = determine_app(obj)
  else
    app = ARGV[0]
  end

  input = <<"END"
INIT
#{obj.to_kyopro}
END
  run(app, input) do |out, err|
    stdout = StringIO.new(out)
    num_futures = stdout.gets.to_i
    futures = []
    for i in 0...num_futures
      source, target = stdout.gets.split().map(&:to_i)
      futures.push({source: source, target: target})
    end
    obj.app_state = stdout.read
    obj.app_name = app
    payload = {
        ready: obj.my_id,
        futures: futures,
        state: obj.to_hash
    }
    print_json(STDOUT, payload)
    #STDERR.puts payload
    # STDERR.puts err
  end
elsif json.key?('move')
  obj = GamePlay.from_json(json)
  input = <<"END"
MOVE
#{obj.to_kyopro}
END
  if ARGV[1] == 'prod'
    app = determine_app(obj)
  else
    app = ARGV[0]
  end
  run(app, input) do |out, err|
    stdout = StringIO.new(out)
    edge_id = stdout.gets.to_i
    case edge_id
      when -1
        move = Move.pass(obj.state.my_id)
      when -2
        site_id_list = stdout.gets.split.map(&:to_i)
        move = Move.splurge(obj.state.my_id,site_id_list.map{|i| obj.state.map.site_of(i)})
        move.route[0..-2].zip(move.route[1..-1]).each do |a, b|
          edge_id = obj.state.map.edge(a, b)
          obj.state.map.set_owner(move.punter, edge_id)
        end
      else
        edge = obj.state.map.edge_of(edge_id)
        if edge.owner1 == -1
          move = Move.claim(obj.state.my_id, edge_id)
        else
          move = Move.option(obj.state.my_id, edge_id)
        end
        obj.state.map.set_owner(obj.state.my_id, edge_id)
    end
    obj.state.app_state = stdout.read
    payload = {
        state: obj.state.to_hash
    }.merge(move.to_hash(obj.state.map))
    print_json(STDOUT, payload)
    # STDERR.puts payload
    # STDERR.puts err
  end
elsif json.key?('stop')
  # STDERR.puts reader.read_json
end
