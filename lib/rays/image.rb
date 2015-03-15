# -*- coding: utf-8 -*-


require 'forwardable'
require 'rays/ext'


module Rays


  class Image

    extend Forwardable

    def_delegators :bitmap, :draw_string, :[], :[]=

    def paint (&block)
      painter.begin self, &block
      self
    end

    def size ()
      return width, height
    end

    def bounds ()
      Bounds.new 0, 0, *size
    end

    def self.load (path, alpha_only = false)
      load_image path, alpha_only
    end

  end# Image


end# Rays
