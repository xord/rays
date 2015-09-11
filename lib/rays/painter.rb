# -*- coding: utf-8 -*-


require 'xot/universal_accessor'
require 'xot/block_util'
require 'rays/ext'


module Rays


  class Painter

    universal_accessor :background, :fill, :stroke, :clip, :font

    def attach (shader, uniforms = {})
      attach_shader shader
      uniforms.each {|name, args| set_uniform name, *args}
      shader
    end

    alias detach detach_shader

    def push (*types, **attributes, &block)
      each_types types do |type|
        case type
        when :matrix  then push_matrix
        when :attrs   then push_attrs
        when :shaders then push_shaders
        else raise ArgumentError, "invalid push/pop type '#{type}'."
        end
      end

      raise ArgumentError, 'missing block with pushing attributes.' if
        !attributes.empty? && !block

      if block
        attributes.each do |key, value|
          attributes[key] = __send__ key
          __send__ key, *value
        end

        Xot::BlockUtil.instance_eval_or_block_call self, &block

        attributes.each do |key, value|
          __send__ key, *value
        end

        pop *types
      end
    end

    def pop (*types)
      each_types types, true do |type|
        case type
        when :matrix  then pop_matrix
        when :attrs   then pop_attrs
        when :shaders then pop_shaders
        else raise ArgumentError, "invalid push/pop type '#{type}'."
        end
      end
    end

    def begin (*args, &block)
      begin_paint
      Xot::BlockUtil.instance_eval_or_block_call self, *args, &block
      end_paint
    end

    private

      def each_types (types, reverse = false, &block)
        types = [:matrix, :attrs, :shaders] if types.include?(:all)
        types = types.reverse if reverse
        types.each &block
      end

  end# Painter


end# Rays
