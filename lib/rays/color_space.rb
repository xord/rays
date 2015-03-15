# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class ColorSpace

    def initialize (type = :RGBA, premultiplied = true)
      setup self.class.get_type(type), premultiplied
    end

    def self.get_type (obj)
      case obj
      when Integer        then obj
      when String, Symbol then const_get obj
      else raise ArgumentError, "can not convert '#{obj}' to ColorSpace type."
      end
    rescue NameError
      raise "ColoeSpace::#{obj} is not found."
    end

    def self.get_color_space (obj)
      (ColorSpace === obj) ? obj : (ColorSpace.new get_type(obj))
    end

  end# ColorSpace


end# Rays
