# -*- coding: utf-8 -*-


require 'xot/universal_accessor'
require 'xot/block_util'
require 'rays/ext'


module Rays


  class Painter

    def push (*types, **attributes, &block)
      each_types types do |type|
        case type
        when :state   then push_state
        when :matrix  then push_matrix
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
      each_types types, reverse: true do |type|
        case type
        when :state   then pop_state
        when :matrix  then pop_matrix
        else raise ArgumentError, "invalid push/pop type '#{type}'."
        end
      end
    end

    def paint (*args, &block)
      begin_paint
      Xot::BlockUtil.instance_eval_or_block_call self, *args, &block
      end_paint
      self
    end

    def line (*args, loop: false)
      case first = args.first
      when Polyline then polyline first
      when Polygon  then polygon  first
      else loop ? line_loop(*args) : line_strip(*args)
      end
    end

    def color= (fill, stroke = nil)
      self.fill   fill
      self.stroke stroke
    end

    def color ()
      return fill, stroke
    end

    def shader= (shader, **uniforms)
      shader.uniform **uniforms if shader && !uniforms.empty?
      set_shader shader
    end

    universal_accessor :background, :fill, :stroke, :color, :shader, :clip, :font

    private

      def each_types (types, reverse: false, &block)
        types = [:state, :matrix] if types.include?(:all)
        types = types.reverse if reverse
        types.each &block
      end

  end# Painter


end# Rays
