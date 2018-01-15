# -*- coding: utf-8 -*-


require_relative 'helper'


class TestShader < Test::Unit::TestCase

  def shader (src)
    Rays::Shader.new src
  end

  def img (w = 10, h = 10, &block)
    Rays::Image.new w, h
  end

  def rgb (*args)
    Rays::Color.new *args
  end

  def test_initialize ()
    assert_raise(Rucy::NativeError) {shader "foo"}
    assert_raise(ArgumentError) {shader}
    #assert_raise(TypeError) {shader nil}
    assert shader("void main() {gl_FragColor = vec4(1, 0, 0, 1);}")
  end

  def test_shader ()
    i = img.paint do
      shader "void main () {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}"
      fill 0, 1, 0, 1
      rect *bounds.move_to(0, 0).to_a
    end
    assert_equal rgb(1, 0, 0, 1), i[0, 0]
  end

end# TestShader
