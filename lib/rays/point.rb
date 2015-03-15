# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class Point

    include Comparable

    def move_to (*args)
      dup.move_to! *args
    end

    def move_by (*args)
      dup.move_by! *args
    end

    def to_a (dimension = 2)
      case dimension
      when 1 then [x]
      when 2 then [x, y]
      when 3 then [x, y, z]
      else raise ArgumentError
      end
    end

    def + (*args)
      op_add Point.from(*args)
    end

    def - (*args)
      op_sub Point.from(*args)
    end

    def * (*args)
      op_mult Point.from(*args)
    end

    def / (*args)
      op_div Point.from(*args)
    end

    def <=> (o)
      ret = x <=> o.x; return ret if ret != 0
      ret = y <=> o.y; return ret if ret != 0
            z <=> o.z
    end

    def self.from (*args)
      case arg0 = args[0]
      when Point   then arg0
      when Array   then Point.new *arg0
      when Numeric then Point.new arg0, arg0, arg0
      else raise ArgumentError
      end
    end

  end# Point


end# Rays
