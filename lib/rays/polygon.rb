# -*- coding: utf-8 -*-


require 'xot/block_util'
require 'rays/ext'


module Rays


  class Polygon

    def self.line (*args, loop: false)
      loop ? line_loop(*args) : line_strip(*args)
    end

  end# Polygon


end# Rays
