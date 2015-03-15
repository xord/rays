# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class Color

    include Comparable

    def self.color (*args)
      c = case arg0 = args[0]
      when Color                  then arg0
      when Symbol                 then COLORS[arg0]
      when /^\s*#[\da-fA-F]+\s*$/ then Color.new arg0
      when String                 then COLORS[arg0.intern]
      else                             Color.new *args
      end
      raise ArgumentError, "invalid argument '#{args}.'" unless c
      c
    end

    def initialize (*args)
      arg0 = args[0]
      args = parse arg0 if arg0 && arg0.kind_of?(String)
      setup *args
    end

    def opaque? ()
      alpha >= 1
    end

    def transparent? ()
      alpha <= 0
    end

    def translucent? ()
      a = alpha
      0 < a && a < 1
    end

    def to_a ()
      [red, green, blue, alpha]
    end

    def [] (index)
      case index
      when 0 then red
      when 1 then green
      when 2 then blue
      when 3 then alpha
      else raise IndexError
      end
    end

    def []= (index, val)
      case index
      when 0 then self.red   = val
      when 1 then self.green = val
      when 2 then self.blue  = val
      when 3 then self.alpha = val
      else raise IndexError
      end
    end

    def <=> (o)
      ret = red   <=> o.red;   return ret if ret != 0
      ret = green <=> o.green; return ret if ret != 0
      ret = blue  <=> o.blue;  return ret if ret != 0
            alpha <=> o.alpha
    end

    def eql? (o)
      self == o
    end

    def inspect ()
      "#<Rays::Color r=#{red}, g=#{green}, b=#{blue}, a=#{alpha}>"
    end

    COLORS = {
      black:   [0, 0, 0],
      red:     [1, 0, 0],
      green:   [0, 1, 0],
      blue:    [0, 0, 1],
      yellow:  [1, 1, 0],
      cyan:    [0, 1, 1],
      magenta: [1, 0, 1],
      white:   [1, 1, 1],
      gray:    [0.5, 0.5, 0.5],
      no:      [0, 0],
      none:    [0, 0],
      nil:     [0, 0],
    }.inject({}) {|h, (k, v)| h[k] = Color.new *v; h}

    private

      RE_RGBA     = /^\s*##{'([\da-fA-F]{1})' * 4}?\s*$/
      RE_RRGGBBAA = /^\s*##{'([\da-fA-F]{2})' * 4}?\s*$/

      def parse (str)
        case str
        when RE_RGBA
          [$1, $2, $3, $4 || 'f' ].map {|s| s.to_i(16) / 15.0}
        when RE_RRGGBBAA
          [$1, $2, $3, $4 || 'ff'].map {|s| s.to_i(16) / 255.0}
        else raise ArgumentError, "can not parse '#{str}'."
        end
      end

  end# Color


end# Rays
