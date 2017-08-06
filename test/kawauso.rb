#!/usr/bin/env ruby

cmd = gets.chomp
case cmd
  when 'INIT'
    np = gets.to_i
    my = gets.to_i
    nv = gets.to_i
    nm = gets.to_i
    mines = gets
    ne = gets.to_i
    ne.times do
      edge = gets
    end
    puts "ho ge fu ga"
    puts "hi ge ha ge"
  when 'MOVE'
    np = gets.to_i
    my = gets.to_i
    nv = gets.to_i
    nm = gets.to_i
    mines = gets
    ne = gets.to_i
    ne.times do
      edge = gets
    end
    puts 1
    puts "this is data"
end
