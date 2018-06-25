# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class Polygon

    include Enumerable

    def initialize (*args)
      add *args
    end

    def add (*args, loop: true, hole: false)
      if args.first.kind_of Polyline
        add_polyline args.first, hole
      else
        add_points args, loop, hole
      end
    end

    def self.line (*args, loop: false)
      new *args, loop: loop, hole: false
    end

  end# Polygon


end# Rays
