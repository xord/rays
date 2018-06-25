# -*- coding: utf-8 -*-


require 'rays/ext'
require 'rays/polyline'


module Rays


  class Polygon

    include Enumerable

    def initialize (*args, loop: true)
      if !args.empty? && args.first.kind_of?(Polyline)
        set_polyline args.first
      else
        set_points args, loop
      end
    end

    def self.line (*args, loop: false)
      new *args, loop: loop
    end

  end# Polygon


end# Rays
