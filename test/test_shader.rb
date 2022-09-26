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

  def test_initialize()
    vs = "void main() {gl_Position = vec4(0.0, 0.0, 0.0, 1.0);}"
    fs = "void main() {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
    assert_raise(Rucy::NativeError) {shader "foo"}
    assert_raise(Rucy::NativeError) {shader "foo", vs}
    assert_raise(Rucy::NativeError) {shader fs, "foo"}
    assert_raise(ArgumentError) {shader nil}
    assert shader(fs)
    assert shader(fs, vs)
  end

  def test_shader()
    i = img.paint do
      shader "void main () {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
      fill 0, 1, 0, 1
      rect(*bounds.move_to(0, 0).to_a)
    end
    assert_equal color(1, 0, 0, 1), i[0, 0]
  end

end# TestShader
