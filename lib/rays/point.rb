# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class Point

    include Comparable
    include Enumerable

    def move_to (*args)
      dup.move_to! *args
    end

    def move_by (*args)
      dup.move_by! *args
    end

    def zero? ()
      length == 0
    end

    def each (&block)
      to_a.each &block
    end

    def to_a (dimension = 2)
      case dimension
      when 1 then [x]
      when 2 then [x, y]
      when 3 then [x, y, z]
      else raise ArgumentError
      end
    end

    def <=> (o)
      ret = x <=> o.x; return ret if ret != 0
      ret = y <=> o.y; return ret if ret != 0
            z <=> o.z
    end

  end# Point


end# Rays
