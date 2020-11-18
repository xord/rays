# -*- coding: utf-8 -*-


require 'xot/block_util'
require 'rays/ext'


module Rays


  class Camera

    def initialize (*args, &block)
      super *args
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

  end# Camera


end# Rays
