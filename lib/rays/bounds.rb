# -*- coding: utf-8 -*-


require 'rays/ext'
require 'rays/point'


module Rays


  class Bounds

    include Comparable
    include Enumerable

    alias w= width=
    alias w  width

    alias h= height=
    alias h  height

    alias d= depth=
    alias d  depth

    alias pos= position=
    alias pos  position

    alias left_top       position
    def  right_top()     position        .move_by(w - 1,     0) end
    def   left_bottom()  position        .move_by(    0, h - 1) end
    def  right_bottom() (position + size).move_by!(  -1,    -1) end

    def  left_top=(*args)    p = Point.new(*args); self.left,  self.top    = p.x, p.y;  left_top end
    def right_top=(*args)    p = Point.new(*args); self.right, self.top    = p.x, p.y; right_top end
    def  left_bottom=(*args) p = Point.new(*args); self.left,  self.bottom = p.x, p.y;  left_bottom end
    def right_bottom=(*args) p = Point.new(*args); self.right, self.bottom = p.x, p.y; right_bottom end

    alias lt   left_top
    alias lt=  left_top=
    alias rt  right_top
    alias rt= right_top=
    alias lb   left_bottom
    alias lb=  left_bottom=
    alias rb  right_bottom
    alias rb= right_bottom=

    def move_to(*args)
      dup.move_to!(*args)
    end

    def move_by(*args)
      dup.move_by!(*args)
    end

    def resize_to(*args)
      dup.resize_to!(*args)
    end

    def resize_by(*args)
      dup.resize_by!(*args)
    end

    def inset_by(*args)
      dup.inset_by!(*args)
    end

    def each(dimension = 2, &block)
      to_a(dimension).each(&block)
    end

    def to_a(dimension = 2)
      # TODO: return [lt, rb]
      case dimension
      when 1 then [x, w]
      when 2 then [x, y, w, h]
      when 3 then [x, y, z, w, h, d]
      else raise ArgumentError
      end
    end

    def <=>(o)
      ret = x <=> o.x; return ret if ret != 0
      ret = y <=> o.y; return ret if ret != 0
      ret = z <=> o.z; return ret if ret != 0
      ret = w <=> o.w; return ret if ret != 0
      ret = h <=> o.h; return ret if ret != 0
            d <=> o.d
    end

  end# Bounds


end# Rays
