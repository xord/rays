# -*- coding: utf-8 -*-


require 'rays/ext'
require 'rays/color_space'


module Rays


  class Texture

    def initialize (width, height, color_space = :RGBA, alpha_only = false)
      setup width, height, ColorSpace.get_color_space(color_space), alpha_only
    end

    def bounds ()
      Bounew.new 0, 0, width, height
    end

  end# Texture


end# Rays
