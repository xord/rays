# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPainterShape < Test::Unit::TestCase

  def color (*args)
    Rays::Color.new *args
  end

  def image (fill = 1, stroke = 0, pixel_density = 1, &block)
    Rays::Image.new(100, 100, Rays::ColorSpace::RGBA, pixel_density).paint {|p|
      p.fill   fill   > 0 ? color(fill)   : nil
      p.stroke stroke > 0 ? color(stroke) : nil
      p.instance_eval &block if block
    }
  end

  def test_line ()
    img = image(0, 1) {line 1, 1, 98, 98}
    assert_equal 0, img[ 0,  0].a
    assert_equal 1, img[ 1,  1].a
    assert_equal 1, img[97, 97].a
    #assert_equal 1, img[98, 98].a
    assert_equal 0, img[99, 99].a
  end

  def test_rect ()
    img = image {rect 1, 1, 98, 98}
    assert_equal 0, img[ 0,  0].a
    assert_equal 1, img[ 1,  1].a
    assert_equal 1, img[98, 98].a
    assert_equal 0, img[99, 99].a
  end

  def test_rect_rounded ()
    img = image {rect 1, 1, 98, 98, 10, 10, 10, 10}
    assert_equal 0, img[ 0,  0].a
    assert_equal 0, img[ 1,  1].a
    assert_equal 1, img[10, 10].a
    assert_equal 1, img[89, 89].a
    assert_equal 0, img[98, 98].a
    assert_equal 0, img[99, 99].a
  end

  def test_ellipse ()
    img = image {ellipse 1, 1, 98, 98}
    assert_equal 0, img[ 0,  0].a
    assert_equal 1, img[50,  1].a
    assert_equal 1, img[50, 50].a
    assert_equal 1, img[ 1, 50].a
    assert_equal 0, img[99, 99].a
  end

end# TestPainter
