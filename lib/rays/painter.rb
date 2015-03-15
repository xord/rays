# -*- coding: utf-8 -*-


require 'xot/block_util'
require 'rays/ext'
require 'rays/color'


module Rays


  class Painter

    def background (*args, &block)
      set_or_get :background=, :get_background, args, block
    end

    def background= (*args)
      send_set :set_background, :get_background, :no_background, args, :color
    end

    def fill (*args, &block)
      set_or_get :fill=, :get_fill, args, block
    end

    def fill= (*args)
      send_set :set_fill, :get_fill, :no_fill, args, :color
    end

    def stroke (*args, &block)
      set_or_get :stroke=, :get_stroke, args, block
    end

    def stroke= (*args)
      send_set :set_stroke, :get_stroke, :no_stroke, args, :color
    end

    def clip (*args, &block)
      set_or_get :clip=, :get_clip, args, block
    end

    def clip= (*args)
      send_set :set_clip, :get_clip, :no_clip, args
    end

    def font (*args, &block)
      set_or_get :font=, :get_font, args, block
    end

    def font= (*args)
      send_set :set_font, :get_font, nil, args
    end

    def color (fill, stroke = nil, &block)
      org = [self.fill, self.stroke]
      self.fill, self.stroke = fill, stroke
      if block
        Xot::BlockUtil.instance_eval_or_block_call self, &block
        self.fill, self.stroke = org
      end
      org
    end

    def attach (shader, uniforms = {})
      shader =
        case shader
        when Shader then shader
        when String then Shader.new shader
        else raise ArgumentError
        end
      attach_shader shader
      uniforms.each {|name, args| set_uniform name, *args}
      shader
    end

    alias detach detach_shader

    def push (&block)
      push_matrix
      push_attr
      push_shader
      if block
        Xot::BlockUtil.instance_eval_or_block_call self, &block
        pop
      end
    end

    def pop ()
      pop_shader
      pop_attr
      pop_matrix
    end

    def begin (*args, &block)
      begin_paint
      Xot::BlockUtil.instance_eval_or_block_call self, *args, &block
      end_paint
    end

    private

      NONES = [:no, :none, nil]

      def send_set (setter, getter, no, args, mode = nil)
        args = args[0] if args[0].kind_of? Array
        raise ArgumentError if args.empty?

        arg0 = args[0]
        if args.size == 1 && NONES.include?(arg0)
          no ? send(no) : send(setter, nil)
        else
          case mode
          when :color then args = Color.color *args
          end
          send setter, *args
        end
        send getter
      end

      def set_or_get (setter, getter, args, block)
        unless args.empty?
          set_or_push setter, getter, args, block
        else
          send getter
        end
      end

      def set_or_push (setter, getter, args, block)
        org = send getter
        send setter, *args
        if block
          Xot::BlockUtil.instance_eval_or_block_call self, &block
          send setter, org
        end
        org
      end

  end# Painter


end# Rays
