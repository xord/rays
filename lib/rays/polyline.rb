# -*- coding: utf-8 -*-


require 'rays/ext'


module Rays


  class Polyline

    include Enumerable

    def inspect ()
      "#<Rays::Polyline #{to_a.join ', '}, loop: #{loop?}>"
    end

  end# Polyline


end# Rays
