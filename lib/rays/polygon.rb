# -*- coding: utf-8 -*-


require 'rays/ext'
require 'rays/polyline'


module Rays


  class Polygon

    include Enumerable

    def initialize (*args, loop: true)
      if args.first.kind_of?(Polyline)
        set_polyline args.first
      else
        set_points args, loop
      end
    end

    def self.line (*args, loop: false)
      new *args, loop: loop
    end

    def self.rect (
      *args, round: nil, lt: nil, rt: nil, lb: nil, rb: nil, nsegment: nil)

      create_rect args, round, lt, rt, lb, rb, nsegment
    end

    def self.ellipse (
      *args, center: nil, radius: nil, hole: nil, from: nil, to: nil,
      nsegment: nil)

      create_ellipse args, center, radius, hole, from, to, nsegment
    end

  end# Polygon


end# Rays
