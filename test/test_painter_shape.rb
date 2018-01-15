# -*- coding: utf-8 -*-


require_relative 'helper'


class TestPainterShape < Test::Unit::TestCase

  def color (*args)
    Rays::Color.new *args
  end

  def image (fill = 1, stroke = 0, pixel_density = 1, &block)
    Rays::Image.new(100, 100, Rays::ColorSpace::RGBA, pixel_density).paint {|p|
      p.background color(0)
      p.fill   fill   > 0 ? color(fill)   : nil
      p.stroke stroke > 0 ? color(stroke) : nil
      p.instance_eval &block if block
    }
  end

  def test_line ()
    img = image 0, 1 {line 1, 1, 98, 98}
    assert_equal color(0), img.bitmap[ 0,  0]
    assert_equal color(1), img.bitmap[ 1,  1]
    assert_equal color(1), img.bitmap[97, 97]
    #assert_equal color(1), img.bitmap[98, 98]
    assert_equal color(0), img.bitmap[99, 99]
  end

  def test_rect ()
    img = image {rect 1, 1, 98, 98}
    assert_equal color(0), img.bitmap[ 0,  0]
    assert_equal color(1), img.bitmap[ 1,  1]
    assert_equal color(1), img.bitmap[98, 98]
    assert_equal color(0), img.bitmap[99, 99]
  end

  def test_rect_rounded ()
    img = image {rect 1, 1, 98, 98, 10, 10, 10, 10}
    assert_equal color(0), img.bitmap[ 0,  0]
    assert_equal color(0), img.bitmap[ 1,  1]
    assert_equal color(1), img.bitmap[10, 10]
    assert_equal color(1), img.bitmap[89, 89]
    assert_equal color(0), img.bitmap[98, 98]
    assert_equal color(0), img.bitmap[99, 99]
  end

  def test_ellipse ()
    img = image {ellipse 1, 1, 98, 98}
    assert_equal color(0), img.bitmap[ 0,  0]
    assert_equal color(1), img.bitmap[50,  1]
    assert_equal color(1), img.bitmap[50, 50]
    assert_equal color(1), img.bitmap[ 1, 50]
    assert_equal color(0), img.bitmap[99, 99]
  end

end# TestPainter
