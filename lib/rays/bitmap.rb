# -*- coding: utf-8 -*-


require 'rays/ext'
require 'rays/color_space'


module Rays


  class Bitmap

    include Enumerable

    def initialize (width, height, color_space = :RGBA)
      setup width, height, ColorSpace.get_color_space(color_space)
    end

    def each ()
      height.times do |y|
        width.times do |x|
          yield self[x, y], x, y
        end
      end
    end

    def bounds ()
      Bounds.new 0, 0, width, height
    end

    def to_a ()
      map {|o| o}
    end

    def []= (x, y, *args)
      args.flatten!
      case args[0]
      when Color
        set_at x, y, args[0]
      when Numeric
        set_at x, y, *args
      end
    end

  end# Bitmap


end# Rays
