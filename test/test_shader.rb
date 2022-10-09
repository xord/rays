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
    "void main() {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
  end

  def test_initialize()
    assert_raise(Rays::OpenGLError) {shader "foo"}
    assert_raise(Rays::OpenGLError) {shader "foo", vshader}
    assert_raise(Rays::OpenGLError) {shader fshader, "foo"}
    assert_raise(Rays::OpenGLError) {shader vshader}
    assert_raise(Rays::OpenGLError) {shader fshader, fshader}
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
    assert_equal vshader,       shader(fshader, vshader).vertex_shader_source
    assert_equal fshader(true), shader(fshader, vshader).fragment_shader_source

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
