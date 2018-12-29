# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class Polyline

    include Enumerable

    def initialize (*args, loop: false)
      set_points args, loop
    end

    def inspect ()
      "#<Rays::Polyline #{to_a.join ', '}, loop: #{loop?}>"
    end

  end# Polyline


end# Rays
