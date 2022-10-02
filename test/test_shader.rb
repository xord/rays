# -*- coding: utf-8 -*-


require_relative 'helper'


class TestShader < Test::Unit::TestCase

  def shader(*args)
    Rays::Shader.new(*args)
  end

  def img(w = 10, h = 10, &block)
    Rays::Image.new w, h
  end

  def color(*args)
    Rays::Color.new(*args)
  end

  def vshader()
    "void main() {gl_Position = vec4(0.0, 0.0, 0.0, 1.0);}"
  end

  def fshader(precision = false)
    s = "void main() {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
    s = "#ifdef GL_ES\nprecision mediump float;\n#endif\n" + s if precision
    s
  end

  def test_initialize()
    assert_raise(Rucy::NativeError) {shader "foo"}
    assert_raise(Rucy::NativeError) {shader "foo", vshader}
    assert_raise(Rucy::NativeError) {shader fshader, "foo"}
    assert_raise(Rucy::NativeError) {shader vshader}
    assert_raise(Rucy::NativeError) {shader fshader, fshader}
    assert_raise(ArgumentError) {shader nil}
    assert shader(fshader)
    assert shader(fshader, vshader)
  end

  def test_shader()
    i = img.paint do
      shader "void main () {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
      fill 0, 1, 0, 1
      rect(*bounds.move_to(0, 0).to_a)
    end
    assert_equal color(1, 0, 0, 1), i[0, 0]
  end

  def test_shader_source()
    assert_equal vshader,      shader(fshader, vshader).vertex_shader_source
    assert_equal fshader(true), shader(fshader, vshader).fragment_shader_source

    assert_true shader(fshader).vertex_shader_source.then {|source|
      %w[
        a_Position a_TexCoord a_Color
        v_Position v_TexCoord v_Color
        u_PositionMatrix u_TexCoordMatrix
      ].all? {|name| source.include? name}
    }

    env = {
      attribute_position:      'attribute_P',
      attribute_texcoord:      'attribute_TC',
      attribute_color:         'attribute_C',
      varying_position:        'varying_P',
      varying_texcoord:        'varying_TC',
      varying_color:           'varying_C',
      uniform_position_matrix: 'uniform_PM',
      uniform_texcoord_matrix: 'uniform_TCM'
    }
    assert_true shader(fshader, nil, env).vertex_shader_source.then {|source|
      %w[
        attribute_P attribute_TC attribute_C
          varying_P   varying_TC   varying_C
        uniform_PM uniform_TCM
      ].all? {|name| source.include? name}
    }
  end

end# TestShader
