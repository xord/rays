# -*- coding: utf-8 -*-


require 'xot/block_util'
require 'rays/ext'


module Rays


  class Shader

    def initialize(source = nil, **uniforms, &block)
      if source
        setup source
        uniform(**uniforms) unless uniforms.empty?
      end

      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def uniform(name = nil, *args, **uniforms)
      set_uniform name, *args if name
      uniforms.each do |key, value|
        set_uniform key, value
      end
    end

  end# Shader


end# Rays
