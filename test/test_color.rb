# -*- coding: utf-8 -*-


require_relative 'helper'


class TestColor < Test::Unit::TestCase

  def color (*args)
    Rays::Color.new *args
  end

  def color8 (r, g, b, a, div = 255)
    color *[r, g, b, a].map {|n| n / div.to_f}
  end

  def to_color (*args)
    Rays::Color.color *args
  end

  def test_initialize ()
    assert_equal color(0, 0, 0, 1), color()
    assert_equal color(1, 1, 1, 1), color(1)
    assert_equal color(1, 1, 1, 2), color(1, 2)
    assert_equal color(1, 2, 3, 1), color(1, 2, 3)
    assert_equal color(1, 2, 3, 4), color(1, 2, 3, 4)
    assert_equal color8(1, 2, 3, 4), color('#01020304')
    assert_raise(ArgumentError) {color(1, 2, 3, 4, 5)}
  end

  def test_color ()
    assert_equal color(0, 0, 0, 1), to_color('#000')
    assert_equal color(0, 0, 0, 1), to_color('#000000')
    assert_equal color(0, 0, 0, 0), to_color('#0000')
    assert_equal color(0, 0, 0, 0), to_color('#00000000')
    assert_equal color8(0x01, 0x23, 0x45, 0x67), to_color('#01234567')
    assert_equal color8(0x89, 0xab, 0xcd, 0xef), to_color('#89abcdef')
    assert_equal color8(0x0, 0x2, 0x4, 0x6, 15), to_color('#0246')
    assert_equal color8(0x9, 0xb, 0xd, 0xf, 15), to_color('#9bdf')
    assert_equal color(0, 0, 0, 1), to_color(' #000 ')
    assert_raise(ArgumentError) {to_color '#'}
    assert_raise(ArgumentError) {to_color '000'}
    assert_raise(ArgumentError) {to_color '#0'}
    assert_raise(ArgumentError) {to_color '#00'}
    assert_raise(ArgumentError) {to_color '#00000'}
    assert_raise(ArgumentError) {to_color '#0000000'}
  end

  def test_get_rgb ()
    o = color 1, 2, 3, 4
    assert_equal 1, o.red
    assert_equal 2, o.green
    assert_equal 3, o.blue
    assert_equal 4, o.alpha
  end

  def test_set_rgb ()
    o = color
    assert_equal [0, 0, 0, 1], o.to_a
    o.red = 1
    assert_equal [1, 0, 0, 1], o.to_a
    o.green = 2
    assert_equal [1, 2, 0, 1], o.to_a
    o.blue = 3
    assert_equal [1, 2, 3, 1], o.to_a
    o.alpha = 4
    assert_equal [1, 2, 3, 4], o.to_a
  end

  def test_to_a ()
    o = color 1, 2, 3, 4
    assert_equal [1, 2, 3, 4], o.to_a
  end

  def test_index ()
    o = color 1, 2, 3, 4
    assert_equal 1, o[0]
    assert_equal 2, o[1]
    assert_equal 3, o[2]
    assert_equal 4, o[3]
    assert_raise(IndexError) {color[-1]}
    assert_raise(IndexError) {color[4]}
  end

  def test_index_assign ()
    o = color 1, 2, 3, 4
    o[0] = 4
    assert_equal [4, 2, 3, 4], o.to_a
    o[1] = 5
    assert_equal [4, 5, 3, 4], o.to_a
    o[2] = 6
    assert_equal [4, 5, 6, 4], o.to_a
    o[3] = 7
    assert_equal [4, 5, 6, 7], o.to_a
    assert_raise(IndexError) {color[-1] = 8}
    assert_raise(IndexError) {color[ 4] = 8}
  end

  def test_compare ()
    o = color 1, 2, 3, 4
    assert     o == color(1, 2, 3, 4)
    assert_not o != color(1, 2, 3, 4)

    assert o < color(2, 2, 3, 4)
    assert o < color(1, 3, 3, 4)
    assert o < color(1, 2, 4, 4)
    assert o < color(1, 2, 3, 5)

    assert o > color(0, 2, 3, 4)
    assert o > color(1, 1, 3, 4)
    assert o > color(1, 2, 2, 4)
    assert o > color(1, 2, 3, 3)
  end

end# TestColor
