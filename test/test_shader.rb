# -*- coding: utf-8 -*-


require_relative 'helper'


class TestShader < Test::Unit::TestCase

  def shader(*args)
    Rays::Shader.new(*args)
  end

  def image(w = 10, h = 10, &block)
    Rays::Image.new w, h
  end

  def color(*args)
    Rays::Color.new(*args)
  end

  def vshader()
    "void main() {gl_Position = vec4(0.0, 0.0, 0.0, 1.0);}"
  end

  def fshader()
    "void main() {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
  end

  def draw_shader(fill = 1, fragment_shader_source, **uniforms, &block)
    image.tap do |img|
      img.paint do |p|
        p.shader fragment_shader_source, **uniforms
        block.call p.shader if block
        p.fill(*fill)
        p.stroke nil
        p.rect img.bounds.move_to(0, 0)
      end
    end
  end

  def test_initialize()
    assert_raise(Rays::ShaderError) {shader "foo"}
    assert_raise(Rays::ShaderError) {shader "foo", vshader}
    assert_raise(Rays::ShaderError) {shader fshader, "foo"}
    assert_raise(Rays::ShaderError) {shader vshader}
    assert_raise(Rays::ShaderError) {shader fshader, fshader}
    assert_raise(ArgumentError) {shader nil}
    assert shader(fshader)
    assert shader(fshader, vshader)
  end

  def test_shader()
    assert_equal color(1, 0, 0, 1), draw_shader([0, 1, 0, 1], <<~END)[0, 0]
      void main() {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}
    END
  end

  def test_uniform_int()
    assert_equal color(1, 0, 0, 1), draw_shader(<<~END) {_1.uniform :v , 1}[0, 0]
      uniform int v;
      void main() {
        gl_FragColor = vec4(float(v), 0.0, 0.0, 1.0);
      }
    END
    assert_equal color(1, 1, 0, 1), draw_shader(<<~END) {_1.uniform :v, 1, 2}[0, 0]
      uniform ivec2 v;
      void main() {
        gl_FragColor = vec4(float(v.x), float(v.y) / 2.0, 0.0, 1.0);
      }
    END
    assert_equal color(1, 1, 0, 1), draw_shader(<<~END) {_1.uniform :v, [1, 2]}[0, 0]
      uniform ivec2 v;
      void main() {
        gl_FragColor = vec4(float(v.x), float(v.y) / 2.0, 0.0, 1.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, 1, 2, 3}[0, 0]
      uniform ivec3 v;
      void main() {
        gl_FragColor = vec4(float(v.x), float(v.y) / 2.0, float(v.z) / 3.0, 1.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, [1, 2, 3]}[0, 0]
      uniform ivec3 v;
      void main() {
        gl_FragColor = vec4(float(v.x), float(v.y) / 2.0, float(v.z) / 3.0, 1.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, 1, 2, 3, 4}[0, 0]
      uniform ivec4 v;
      void main() {
        gl_FragColor = vec4(float(v.x), float(v.y) / 2.0, float(v.z) / 3.0, float(v.w) / 4.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, [1, 2, 3, 4]}[0, 0]
      uniform ivec4 v;
      void main() {
        gl_FragColor = vec4(float(v.x), float(v.y) / 2.0, float(v.z) / 3.0, float(v.w) / 4.0);
      }
    END
  end

  def test_uniform_float()
    assert_equal color(1, 0, 0, 1), draw_shader(<<~END) {_1.uniform :v , 1.0}[0, 0]
      uniform float v;
      void main() {
        gl_FragColor = vec4(v, 0.0, 0.0, 1.0);
      }
    END
    assert_equal color(1, 1, 0, 1), draw_shader(<<~END) {_1.uniform :v, 1.0, 2.0}[0, 0]
      uniform vec2 v;
      void main() {
        gl_FragColor = vec4(v.x, v.y / 2.0, 0.0, 1.0);
      }
    END
    assert_equal color(1, 1, 0, 1), draw_shader(<<~END) {_1.uniform :v, [1.0, 2.0]}[0, 0]
      uniform vec2 v;
      void main() {
        gl_FragColor = vec4(v.x, v.y / 2.0, 0.0, 1.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, 1.0, 2.0, 3.0}[0, 0]
      uniform vec3 v;
      void main() {
        gl_FragColor = vec4(v.x, v.y / 2.0, v.z / 3.0, 1.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, [1.0, 2.0, 3.0]}[0, 0]
      uniform vec3 v;
      void main() {
        gl_FragColor = vec4(v.x, v.y / 2.0, v.z / 3.0, 1.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, 1.0, 2.0, 3.0, 4.0}[0, 0]
      uniform vec4 v;
      void main() {
        gl_FragColor = vec4(v.x, v.y / 2.0, v.z / 3.0, v.w / 4.0);
      }
    END
    assert_equal color(1, 1, 1, 1), draw_shader(<<~END) {_1.uniform :v, [1.0, 2.0, 3.0, 4.0]}[0, 0]
      uniform vec4 v;
      void main() {
        gl_FragColor = vec4(v.x, v.y / 2.0, v.z / 3.0, v.w / 4.0);
      }
    END
  end

  def test_uniform_error()
    assert_raise(Rays::ShaderError) {draw_shader('void main() {}', val: 1.0)}
  end

  def test_shader_source()
    assert_equal vshader, shader(fshader, vshader).vertex_shader_source
    assert_equal fshader, shader(fshader, vshader).fragment_shader_source

    assert_true shader(fshader).vertex_shader_source.then {|source|
      %w[
        a_Position a_TexCoord a_Color
        v_Position v_TexCoord v_Color
        u_PositionMatrix u_TexCoordMatrix
      ].all? {|name| source.include? name}
    }
  end

  def test_shader_env()
    env = {
      attribute_position:       'aP',
      attribute_texcoord:       ['aTC', 'aTC2'],
      attribute_color:          [],
      varying_position:         'vP',
      varying_texcoord:         nil,
      #varying_color:           define nothing
      uniform_position_matrix:  nil,
      #uniform_texcoord_matrix: define nothing
    }
    assert_true shader(fshader, nil, env).vertex_shader_source.then {|source|
      %w[
        aP
        aTC
        a_Color
        vP
        v_TexCoord
        v_Color
        u_PositionMatrix
        u_TexCoordMatrix
      ].all? {|name| source.include? name}
    }

    assert_raise(ArgumentError) {shader(fshader, nil, {attribute_color: ''})}
    assert_raise(ArgumentError) {shader(fshader, nil, {attribute_color: ['']})}
    assert_raise(ArgumentError) {shader(fshader, nil, {attribute_color: [nil]})}
    assert_raise(ArgumentError) {shader(fshader, nil, {attribute_color: ['C', '']})}
    assert_raise(ArgumentError) {shader(fshader, nil, {attribute_color: ['C', nil]})}

    assert_raise(ArgumentError) {shader(fshader, nil, {varying_color: ''})}
  end

end# TestShader
