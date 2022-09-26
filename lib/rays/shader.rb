# -*- coding: utf-8 -*-


require 'xot/block_util'
require 'rays/ext'


module Rays


  class Shader

    def initialize(
      fragment_shader_source,
      vertex_shader_source = nil,
      **uniforms, &block)

      setup fragment_shader_source, vertex_shader_source
      uniform(**uniforms) unless uniforms.empty?

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
